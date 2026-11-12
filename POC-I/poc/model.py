"""Prova de conceito do modelo (objetivo 6): política gulosa aprendida.

Formulação (seção 7 do relatório da análise exploratória):
  * a cada passo, as 4 heurísticas sugerem candidatos (até 4 vértices distintos),
    exatamente como nos rollouts da camada B;
  * um classificador binário estima, para cada candidato, a probabilidade de ele
    ser ótimo (rótulo 3b);
  * a política escolhe o candidato de maior probabilidade (desempate pela ordem
    dsatur > rlf > welsh_powell > random, a mesma do oráculo) e o colore com a
    menor cor disponível.

O treino usa apenas os passos INFORMATIVOS da camada B (ao menos um candidato com
rótulo 0). Os atributos de cada candidato são calculados por `candidate_features`,
a mesma função usada no treino (sobre a tabela de rollouts) e na política (sobre o
estado corrente), o que garante que o modelo veja atributos idênticos nos dois casos.
"""
from __future__ import annotations

import time
from dataclasses import dataclass

import numpy as np
import pandas as pd
from scipy.stats import rankdata

from .features import graph_features, vertex_features
from .heuristics import HEURISTICS, make_policy
from .rollout import PRIORITY
from .state import ColoringState

CAND_RAW = ("degree", "saturation", "uncolored_nb", "colored_nb", "avail_color",
            "opens_new_color")
CONTEXT = ("n_colors_before", "step_frac", "n_candidates")
# um atributo global por bloco redundante (ver seção 3 do relatório da EDA)
GLOBAL_FEATURES = ("n", "density", "deg_cv", "clust_ratio")

FEATURE_NAMES = (
    *CAND_RAW, *CONTEXT,
    "avail_rel", "sat_rel", "unc_frac", "deg_rel",
    *(f"{f}_rank" for f in CAND_RAW), *(f"{f}_dmin" for f in CAND_RAW),
    *(f"by_{h}" for h in HEURISTICS),
    *(f"g_{f}" for f in GLOBAL_FEATURES),
)


def global_features(A: np.ndarray) -> dict[str, float]:
    """Atributos globais usados pelo modelo (mesmas definições da EDA)."""
    return global_features_from(graph_features(A))


def global_features_from(g: dict) -> dict[str, float]:
    """Mesmo que global_features, a partir da saída de features.graph_features."""
    return {"n": g["n"], "density": g["density"],
            "deg_cv": g["deg_std"] / g["deg_mean"] if g["deg_mean"] else 0.0,
            "clust_ratio": g["avg_clustering"] / g["density"] if g["density"] else 0.0}


def _group_stats(v: np.ndarray, group: np.ndarray | None) -> tuple[np.ndarray, np.ndarray]:
    """(posto percentual médio, valor − mínimo) de v dentro de cada grupo.

    group=None significa um único grupo (caminho rápido usado pela política).
    """
    v = v.astype(np.float64)
    if group is None:
        return rankdata(v, method="average") / len(v), v - v.min()
    s = pd.Series(v)
    g = s.groupby(group)
    return (g.rank(method="average", pct=True).to_numpy(),
            (s - g.transform("min")).to_numpy())


def candidate_features(raw: dict[str, np.ndarray], group: np.ndarray | None,
                       suggested_by: np.ndarray, gfeat: dict[str, np.ndarray | float]
                       ) -> np.ndarray:
    """Matriz de atributos (linhas = candidatos, colunas = FEATURE_NAMES).

    raw          colunas CAND_RAW + CONTEXT (arrays do mesmo tamanho)
    group        id do passo de cada linha (None = todas do mesmo passo)
    suggested_by strings "dsatur|rlf|..." com as heurísticas que sugeriram o candidato
    gfeat        atributos globais (escalares ou arrays por linha)
    """
    m = len(suggested_by)
    f = lambda k: np.asarray(raw[k], dtype=np.float64)   # noqa: E731
    ncb = np.maximum(f("n_colors_before"), 1.0)
    n = np.broadcast_to(np.asarray(gfeat["n"], dtype=np.float64), (m,))
    cols = [f(k) for k in CAND_RAW] + [f(k) for k in CONTEXT]
    cols += [f("avail_color") / ncb, f("saturation") / ncb,
             f("uncolored_nb") / np.maximum(f("degree"), 1.0), f("degree") / n]
    ranks, dmins = zip(*(_group_stats(f(k), group) for k in CAND_RAW))
    cols += list(ranks) + list(dmins)
    sb = pd.Series(suggested_by, dtype=object)
    cols += [sb.str.contains(h, regex=False).to_numpy(dtype=np.float64) for h in HEURISTICS]
    cols += [np.broadcast_to(np.asarray(gfeat[k], dtype=np.float64), (m,))
             for k in GLOBAL_FEATURES]
    return np.column_stack(cols)


def training_table(rollouts: pd.DataFrame, graphs: pd.DataFrame,
                   informative_only: bool = True) -> tuple[np.ndarray, np.ndarray, pd.DataFrame]:
    """(X, y, meta) a partir da tabela de rollouts da camada B.

    graphs: uma linha por grafo com graph_id e as colunas de GLOBAL_FEATURES.
    meta:   graph_id, step, step_id, suggested_by, label, informative (mesma ordem de X).
    """
    R = rollouts.merge(graphs[["graph_id", *GLOBAL_FEATURES]], on="graph_id",
                       validate="many_to_one")
    R["step_id"] = (pd.factorize(R["graph_id"])[0].astype(np.int64) * 100_000 + R["step"])
    R["informative"] = R.groupby("step_id")["label"].transform("min") == 0
    if informative_only:
        R = R[R["informative"]].reset_index(drop=True)
    X = candidate_features({k: R[k].to_numpy() for k in (*CAND_RAW, *CONTEXT)},
                           R["step_id"].to_numpy(), R["suggested_by"].to_numpy(),
                           {k: R[k].to_numpy() for k in GLOBAL_FEATURES})
    meta = R[["graph_id", "step", "step_id", "suggested_by", "label", "informative"]]
    return X, R["label"].to_numpy(), meta


def pick_per_step(score: np.ndarray, step_id: np.ndarray) -> np.ndarray:
    """Máscara com o 1º candidato de maior escore em cada passo (linhas na ordem
    dos candidatos, que já segue a prioridade dsatur > rlf > welsh_powell > random)."""
    df = pd.DataFrame({"s": score, "g": step_id, "i": np.arange(len(score))})
    best = df.sort_values(["g", "s", "i"], ascending=[True, False, True]).drop_duplicates("g")
    mask = np.zeros(len(score), dtype=bool)
    mask[best["i"].to_numpy()] = True
    return mask


# --------------------------------------------------------------------------- #
# Política aprendida
# --------------------------------------------------------------------------- #
@dataclass
class PolicyResult:
    colors: np.ndarray
    order: np.ndarray
    n_colors: int
    seconds: float
    model_calls: int                 # passos em que houve mais de 1 candidato
    follows: dict[str, int]          # nº de passos em que seguiu cada heurística


class LearnedPolicy:
    """Regra de escolha: candidato de maior probabilidade segundo o modelo."""

    def __init__(self, model, A: np.ndarray, seed: int, gfeat: dict[str, float] | None = None):
        self.model = model
        self.policies = {h: make_policy(h, A, seed) for h in PRIORITY}
        self.gfeat = gfeat if gfeat is not None else global_features(A)
        self.model_calls = 0
        self.follows = {h: 0 for h in HEURISTICS}

    def __call__(self, s: ColoringState) -> int:
        cands: dict[int, list[str]] = {}
        for h in PRIORITY:
            cands.setdefault(self.policies[h](s), []).append(h)
        vs = list(cands)
        if len(vs) == 1:
            v = vs[0]
        else:
            vf = vertex_features(s, np.asarray(vs, dtype=np.int64))
            vf["n_candidates"] = np.full(len(vs), len(vs))
            X = candidate_features(vf, None, np.array(["|".join(cands[v]) for v in vs]),
                                   self.gfeat)
            p = self.model.predict_proba(X)[:, 1]
            v = vs[int(np.argmax(p))]          # 1º máximo = desempate por prioridade
            self.model_calls += 1
        for h in cands[v]:
            self.follows[h] += 1
        return v


def run_learned_policy(model, A: np.ndarray, seed: int,
                       gfeat: dict[str, float] | None = None) -> PolicyResult:
    t0 = time.perf_counter()
    pol = LearnedPolicy(model, A, seed, gfeat)
    s = ColoringState(A)
    order = []
    while not s.done:
        v = pol(s)
        s.assign(v)
        order.append(v)
    return PolicyResult(s.color.copy(), np.asarray(order, dtype=np.int32), s.n_colors,
                        time.perf_counter() - t0, pol.model_calls, pol.follows)
