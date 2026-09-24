"""Rótulos por passo via rollout (camada B, "opção 3b").

Em cada passo da construção:
  1. cada heurística de referência indica um candidato (até 4 vértices distintos);
  2. cada candidato distinto é colorido e a coloração é COMPLETADA DUAS VEZES,
     uma com o DSATUR e outra com o RLF (políticas-base);
  3. valor do candidato = menor número de cores entre as duas conclusões;
  4. rótulo = 1 para os candidatos que atingem o valor mínimo (empates são todos 1);
  5. a trajetória segue o melhor candidato (desempate pela prioridade
     dsatur > rlf > welsh_powell > random).

Por que duas políticas-base (versão 2)?
  Na versão 1 só o DSATUR completava os rollouts. O piloto mostrou que, em grafos
  densos, o RLF é bem melhor que o DSATUR, e a coloração oráculo ficava pior que a
  melhor heurística em 33% dos grafos. Os rótulos ensinariam um "DSATUR
  melhorado", e não algo capaz de superar as heurísticas de referência.
  Resultados da versão 1: results/pilot/v1_rollout_somente_dsatur/.

Garantia (melhoria sequencial dos rollouts, Bertsekas):
  Seja H(s) = min(DSATUR(s), RLF(s)) o número de cores obtido completando o estado s
  com a melhor das duas políticas-base. Ambas são determinísticas e dependem só do
  estado, logo são "sequencialmente consistentes": se h escolhe v em s, então
  h(s + v) = h(s). Como os candidatos do DSATUR e do RLF estão sempre entre os
  testados, o melhor candidato vale no máximo H(s). Portanto o valor nunca aumenta
  ao longo da trajetória e a coloração oráculo usa no máximo min(DSATUR, RLF) cores.

Economia: pela mesma consistência, D(s + d) = D(s) para o candidato d do DSATUR e
R(s + r) = R(s) para o candidato r do RLF. Esses dois valores são herdados do passo
anterior, poupando 2 conclusões por passo (verificável com verify=True).
"""
from __future__ import annotations

import time
from dataclasses import dataclass

import numpy as np

from .features import vertex_features
from .heuristics import COMPLETERS, make_policy
from .state import ColoringState

PRIORITY = ("dsatur", "rlf", "welsh_powell", "random")
BASES = ("dsatur", "rlf")            # políticas-base usadas para completar


@dataclass
class RolloutResult:
    rows: dict[str, np.ndarray]     # uma linha por (passo, candidato distinto)
    colors: np.ndarray              # coloração oráculo
    order: np.ndarray
    n_colors: int
    n_rollouts: int                 # nº de conclusões efetivamente calculadas
    seconds: float


def _complete(s: ColoringState, v: int, base: str) -> int:
    s2 = s.copy()
    s2.assign(v)
    return COMPLETERS[base](s2)


def rollout_trajectory(A: np.ndarray, seed: int = 0, verify: bool = False) -> RolloutResult:
    t0 = time.perf_counter()
    policies = {h: make_policy(h, A, seed) for h in PRIORITY}
    s = ColoringState(A)
    buf: dict[str, list] = {k: [] for k in (
        "step", "vertex", "suggested_by", "n_candidates",
        "rollout_dsatur", "rollout_rlf", "rollout_colors", "label", "chosen")}
    feat_buf: list[dict[str, np.ndarray]] = []
    order = []
    n_rollouts = 0
    inherited: dict[str, int] | None = None     # {base: base(s)} do estado atual

    while not s.done:
        cands: dict[int, list[str]] = {}
        for h in PRIORITY:
            cands.setdefault(policies[h](s), []).append(h)

        vals: dict[int, dict[str, int]] = {}
        for v, hs in cands.items():
            vals[v] = {}
            for b in BASES:
                if inherited is not None and b in hs and not verify:
                    vals[v][b] = inherited[b]           # consistência sequencial
                    continue
                vals[v][b] = _complete(s, v, b)
                n_rollouts += 1
                if verify and inherited is not None and b in hs:
                    assert vals[v][b] == inherited[b], "consistência sequencial violada"
        value = {v: min(d.values()) for v, d in vals.items()}
        best = min(value.values())
        chosen = next(v for v in cands if value[v] == best)

        vs = np.fromiter(cands, dtype=np.int64)
        feat_buf.append(vertex_features(s, vs))
        for v, hs in cands.items():
            buf["step"].append(s.step)
            buf["vertex"].append(v)
            buf["suggested_by"].append("|".join(hs))
            buf["n_candidates"].append(len(cands))
            buf["rollout_dsatur"].append(vals[v]["dsatur"])
            buf["rollout_rlf"].append(vals[v]["rlf"])
            buf["rollout_colors"].append(value[v])
            buf["label"].append(int(value[v] == best))
            buf["chosen"].append(int(v == chosen))

        s.assign(chosen)
        order.append(chosen)
        inherited = vals[chosen]            # D(s') e R(s') do novo estado

    rows = {k: np.asarray(v) for k, v in buf.items()}
    for k in feat_buf[0]:
        rows[k] = np.concatenate([f[k] for f in feat_buf])
    return RolloutResult(rows, s.color.copy(), np.array(order, dtype=np.int32),
                         s.n_colors, n_rollouts, time.perf_counter() - t0)
