"""Heurísticas gulosas de referência para coloração de vértices.

Cada heurística é definida por uma REGRA DE ESCOLHA (policy): uma função
    policy(estado: ColoringState) -> vértice
que depende apenas do estado parcial atual. A cor atribuída é sempre a menor
disponível (ver state.py). Isso vale inclusive para o RLF: como o RLF fecha
cada classe de cor de forma maximal antes de abrir a próxima, a cor que ele
atribui coincide com a menor cor disponível (verificado nos testes).

Regras implementadas (desempates sempre pelo menor índice de vértice):
  random        ordem aleatória fixa (guloso simples)
  welsh_powell  grau decrescente (Welsh & Powell, 1967)
  dsatur        maior saturação; desempate: maior grau no subgrafo não
                colorido (Brélaz, 1979)
  rlf           Recursive Largest First (Leighton, 1979): constrói uma classe
                de cor por vez. Primeiro vértice da classe: maior grau no
                subgrafo não colorido. Seguintes: dentre os vértices que ainda
                podem entrar na classe (U), o que tem mais vizinhos entre os
                que já não podem (W); desempate: menos vizinhos em U.

Para o RLF há duas implementações:
  * rlf_choice: regra "sem memória", aplicável a QUALQUER estado parcial
    (necessária para os rollouts, em que regras diferentes se misturam);
  * rlf: versão incremental, bem mais rápida, para execuções completas.
Os testes garantem que ambas produzem exatamente a mesma ordem.
"""
from __future__ import annotations

import time
from dataclasses import dataclass, field
from typing import Callable

import numpy as np

from .state import ColoringState

Policy = Callable[[ColoringState], int]

HEURISTICS = ("random", "welsh_powell", "dsatur", "rlf")


@dataclass
class ColoringResult:
    heuristic: str
    colors: np.ndarray          # cor de cada vértice (0..k-1)
    order: np.ndarray           # ordem em que os vértices foram coloridos
    n_colors: int
    seconds: float = 0.0
    extra: dict = field(default_factory=dict)


# --------------------------------------------------------------------------- #
# Utilitários
# --------------------------------------------------------------------------- #
def _masked_argmax(key: np.ndarray, mask: np.ndarray) -> int:
    """Índice do maior key entre as posições com mask=True (1º em caso de empate)."""
    return int(np.argmax(np.where(mask, key, -1)))


def is_valid_coloring(A: np.ndarray, colors: np.ndarray) -> bool:
    """Coloração própria: todos coloridos e nenhuma aresta monocromática."""
    colors = np.asarray(colors)
    if colors.shape != (A.shape[0],) or (colors < 0).any():
        return False
    for c in np.unique(colors):
        idx = np.flatnonzero(colors == c)
        if A[np.ix_(idx, idx)].any():
            return False
    return True


# --------------------------------------------------------------------------- #
# Regras de escolha (policies)
# --------------------------------------------------------------------------- #
class StaticOrderPolicy:
    """Escolhe o primeiro vértice não colorido de uma ordem fixa."""

    def __init__(self, order: np.ndarray):
        n = len(order)
        self.rank = np.empty(n, dtype=np.int64)
        self.rank[np.asarray(order)] = np.arange(n)
        self._big = n + 1

    def __call__(self, s: ColoringState) -> int:
        return int(np.argmin(np.where(s.uncolored, self.rank, self._big)))


def random_policy(A: np.ndarray, seed: int) -> StaticOrderPolicy:
    rng = np.random.default_rng(seed)
    return StaticOrderPolicy(rng.permutation(A.shape[0]))


def welsh_powell_policy(A: np.ndarray) -> StaticOrderPolicy:
    deg = A.sum(axis=1)
    return StaticOrderPolicy(np.argsort(-deg, kind="stable"))


def dsatur_choice(s: ColoringState) -> int:
    n = s.n
    key = s.sat.astype(np.int64) * (n + 1) + s.udeg
    return _masked_argmax(key, s.uncolored)


def rlf_choice(s: ColoringState) -> int:
    """Regra do RLF aplicável a um estado parcial arbitrário.

    k = menor cor que algum vértice não colorido ainda pode receber.
    Se a classe k está vazia (cor nova), escolhe o vértice de maior grau no
    subgrafo não colorido; senão aplica o critério U/W do RLF.
    """
    n, unc, nc = s.n, s.uncolored, s.n_colors
    if nc:
        rows = (~s.has[:nc] & unc).any(axis=1)
        k = int(np.argmax(rows)) if rows.any() else nc
    else:
        k = 0
    U = unc & ~s.has[k]
    if k == nc:                                   # classe k ainda vazia
        return _masked_argmax(s.udeg.astype(np.int64), U)
    W = unc & s.has[k]
    u_idx = np.flatnonzero(U)
    sub = s.A[u_idx]
    n_w = (sub & W).sum(axis=1).astype(np.int64)
    n_u = (sub & U).sum(axis=1).astype(np.int64)
    return int(u_idx[np.argmax(n_w * (n + 1) + (n - n_u))])


def make_policy(name: str, A: np.ndarray, seed: int = 0) -> Policy:
    if name == "random":
        return random_policy(A, seed)
    if name == "welsh_powell":
        return welsh_powell_policy(A)
    if name == "dsatur":
        return dsatur_choice
    if name == "rlf":
        return rlf_choice
    raise ValueError(f"heurística desconhecida: {name}")


# --------------------------------------------------------------------------- #
# Execução completa
# --------------------------------------------------------------------------- #
def run_policy(A: np.ndarray, policy: Policy, name: str = "policy",
               state: ColoringState | None = None) -> ColoringResult:
    """Completa a coloração a partir de `state` (ou do zero) usando `policy`."""
    t0 = time.perf_counter()
    s = ColoringState(A) if state is None else state
    order = []
    while not s.done:
        v = policy(s)
        s.assign(v)
        order.append(v)
    return ColoringResult(name, s.color.copy(), np.array(order, dtype=np.int32),
                          s.n_colors, time.perf_counter() - t0)


def rlf(A: np.ndarray) -> ColoringResult:
    """RLF incremental: O(k * n^2) operações vetorizadas (k = nº de cores)."""
    t0 = time.perf_counter()
    n = A.shape[0]
    color = np.full(n, -1, dtype=np.int32)
    unc = np.ones(n, dtype=bool)
    udeg = A.sum(axis=1).astype(np.int64)
    order: list[int] = []
    k = 0
    while unc.any():
        U = unc.copy()
        n_u = udeg.copy()                    # em U = não coloridos, |N(v)∩U| = udeg
        n_w = np.zeros(n, dtype=np.int64)
        v = _masked_argmax(udeg, U)
        while True:
            color[v] = k
            unc[v] = False
            U[v] = False
            order.append(v)
            row = A[v]
            udeg -= row
            n_u -= row
            moved = row & U                  # vizinhos de v saem de U e vão para W
            if moved.any():
                U &= ~moved
                cnt = A[moved].sum(axis=0)
                n_u -= cnt
                n_w += cnt
            if not U.any():
                break
            v = _masked_argmax(n_w * (n + 1) + (n - n_u), U)
        k += 1
    return ColoringResult("rlf", color, np.array(order, dtype=np.int32), k,
                          time.perf_counter() - t0)


def rlf_complete(s: ColoringState) -> int:
    """Completa IN PLACE o estado parcial `s` seguindo o RLF e retorna o nº de cores.

    Produz exatamente a mesma sequência que aplicar `rlf_choice` passo a passo
    (verificado nos testes), mas mantém as contagens |N(u)∩U| e |N(u)∩W|
    incrementalmente dentro de cada classe, como a função `rlf`. É usada como
    política-base dos rollouts, a partir de estados parciais arbitrários.
    """
    A, n = s.A, s.n
    while not s.done:
        nc, unc = s.n_colors, s.uncolored
        if nc:
            rows = (~s.has[:nc] & unc).any(axis=1)
            k = int(np.argmax(rows)) if rows.any() else nc
        else:
            k = 0
        U = unc & ~s.has[k]
        if k == nc:                                  # classe nova: maior grau não colorido
            v = _masked_argmax(s.udeg.astype(np.int64), U)
        else:
            v = -1
        W = unc & s.has[k]
        n_u = A[:, U].sum(axis=1, dtype=np.int64)
        n_w = A[:, W].sum(axis=1, dtype=np.int64)
        while True:
            if v < 0:
                v = _masked_argmax(n_w * (n + 1) + (n - n_u), U)
            s.assign(v, k)
            U[v] = False
            row = A[v]
            n_u -= row
            moved = row & U                          # vizinhos de v saem de U para W
            if moved.any():
                U &= ~moved
                cnt = A[moved].sum(axis=0)
                n_u -= cnt
                n_w += cnt
            if not U.any():
                break
            v = -1
    return s.n_colors


def dsatur_complete(s: ColoringState) -> int:
    """Completa IN PLACE o estado parcial `s` seguindo o DSATUR e retorna o nº de cores."""
    while not s.done:
        s.assign(dsatur_choice(s))
    return s.n_colors


COMPLETERS = {"dsatur": dsatur_complete, "rlf": rlf_complete}


def rlf_complete(s: ColoringState) -> int:
    """Completa, IN PLACE, a coloração do estado `s` com a regra do RLF.

    Produz exatamente a mesma sequência que aplicar `rlf_choice` passo a passo
    (verificado nos testes), mas de forma incremental: os contadores |N(u)∩W| e
    |N(u)∩U| são recalculados só no início de cada classe e depois atualizados.
    Usado nos rollouts, em que o RLF parte de estados parciais arbitrários.
    Retorna o número final de cores.
    """
    A, n = s.A, s.n
    while not s.done:
        nc = s.n_colors
        rows = (~s.has[:nc] & s.uncolored).any(axis=1) if nc else np.zeros(0, bool)
        k = int(np.argmax(rows)) if rows.any() else nc
        U = s.uncolored & ~s.has[k]
        if k == nc:                                   # classe nova: maior grau não colorido
            v = _masked_argmax(s.udeg.astype(np.int64), U)
            s.assign(v)
            U = s.uncolored & ~s.has[k]
        W = s.uncolored & s.has[k]
        n_u = A[:, U].sum(axis=1, dtype=np.int64)
        n_w = A[:, W].sum(axis=1, dtype=np.int64)
        while U.any():
            v = _masked_argmax(n_w * (n + 1) + (n - n_u), U)
            s.assign(v)
            row = A[v]
            U[v] = False
            n_u -= row
            moved = row & U                           # vizinhos de v passam de U para W
            if moved.any():
                U &= ~moved
                cnt = A[moved].sum(axis=0, dtype=np.int64)
                n_u -= cnt
                n_w += cnt
    return s.n_colors


def dsatur_complete(s: ColoringState) -> int:
    """Completa, IN PLACE, a coloração do estado `s` com o DSATUR."""
    while not s.done:
        s.assign(dsatur_choice(s))
    return s.n_colors


COMPLETERS = {"dsatur": dsatur_complete, "rlf": rlf_complete}


def run_heuristic(name: str, A: np.ndarray, seed: int = 0) -> ColoringResult:
    """Executa uma heurística completa sobre o grafo A."""
    if name == "rlf":
        return rlf(A)
    return run_policy(A, make_policy(name, A, seed), name)
