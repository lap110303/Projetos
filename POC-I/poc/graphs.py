"""Geração de grafos sintéticos (Erdős–Rényi e Barabási–Albert).

Representação: matriz de adjacência densa, simétrica, booleana, com diagonal
falsa (np.ndarray de shape (n, n), dtype=bool).

Cada grafo é identificado por uma string estável (ex.: "er_n500_p0.35_s2") e a
semente do gerador é derivada desse identificador por hash, de modo que:
  * o mesmo id sempre gera o mesmo grafo (reprodutibilidade);
  * grafos diferentes usam fluxos aleatórios independentes.
"""
from __future__ import annotations

import hashlib
from dataclasses import dataclass

import networkx as nx
import numpy as np


# --------------------------------------------------------------------------- #
# Identificadores e sementes
# --------------------------------------------------------------------------- #
@dataclass(frozen=True)
class GraphSpec:
    """Especificação de um grafo sintético."""
    model: str          # "er" ou "ba"
    n: int
    param: float        # p (ER) ou m (BA)
    seed: int           # índice da réplica (0, 1, 2, ...)

    @property
    def graph_id(self) -> str:
        if self.model == "er":
            return f"er_n{self.n}_p{self.param:.2f}_s{self.seed}"
        if self.model == "ba":
            return f"ba_n{self.n}_m{int(self.param)}_s{self.seed}"
        raise ValueError(f"modelo desconhecido: {self.model}")


def stable_seed(*parts: object) -> int:
    """Semente inteira de 63 bits derivada de forma estável das partes dadas."""
    h = hashlib.sha256("|".join(map(str, parts)).encode()).digest()
    return int.from_bytes(h[:8], "little") & ((1 << 63) - 1)


# --------------------------------------------------------------------------- #
# Geradores
# --------------------------------------------------------------------------- #
def erdos_renyi(n: int, p: float, seed: int) -> np.ndarray:
    """G(n, p): cada par de vértices é ligado independentemente com prob. p.

    Implementação própria em numpy, linha a linha. É muito mais rápida que
    networkx.gnp_random_graph para grafos densos e grandes, e usa apenas a
    memória da matriz final (n^2 bytes).
    """
    rng = np.random.default_rng(seed)
    A = np.zeros((n, n), dtype=bool)
    for i in range(n - 1):
        A[i, i + 1:] = rng.random(n - i - 1) < p
    A |= A.T
    return A


def barabasi_albert(n: int, m: int, seed: int) -> np.ndarray:
    """Modelo de ligação preferencial de Barabási–Albert (via networkx).

    Cada novo vértice se liga a m vértices existentes com probabilidade
    proporcional ao grau. Número de arestas: m * (n - m).
    """
    if not 1 <= m < n:
        raise ValueError("BA exige 1 <= m < n")
    G = nx.barabasi_albert_graph(n, m, seed=seed % (2**32))
    return from_edges(n, np.asarray(list(G.edges()), dtype=np.int64).reshape(-1, 2))


def generate(spec: GraphSpec) -> np.ndarray:
    """Gera o grafo correspondente à especificação."""
    s = stable_seed(spec.graph_id)
    if spec.model == "er":
        return erdos_renyi(spec.n, spec.param, s)
    if spec.model == "ba":
        return barabasi_albert(spec.n, int(spec.param), s)
    raise ValueError(spec.model)


# --------------------------------------------------------------------------- #
# Conversões e utilitários
# --------------------------------------------------------------------------- #
def from_edges(n: int, edges: np.ndarray) -> np.ndarray:
    A = np.zeros((n, n), dtype=bool)
    if len(edges):
        A[edges[:, 0], edges[:, 1]] = True
        A[edges[:, 1], edges[:, 0]] = True
    np.fill_diagonal(A, False)
    return A


def from_networkx(G: nx.Graph) -> np.ndarray:
    """Converte um grafo networkx (vértices relabelados 0..n-1 na ordem de G)."""
    idx = {v: i for i, v in enumerate(G.nodes())}
    edges = np.array([(idx[u], idx[v]) for u, v in G.edges() if u != v],
                     dtype=np.int64).reshape(-1, 2)
    return from_edges(len(idx), edges)


def to_networkx(A: np.ndarray) -> nx.Graph:
    G = nx.Graph()
    G.add_nodes_from(range(A.shape[0]))
    iu, ju = np.nonzero(np.triu(A, 1))
    G.add_edges_from(zip(iu.tolist(), ju.tolist()))
    return G


def num_edges(A: np.ndarray) -> int:
    return int(A.sum()) // 2


def check_adjacency(A: np.ndarray) -> None:
    """Valida o formato da matriz de adjacência (lança AssertionError)."""
    assert A.ndim == 2 and A.shape[0] == A.shape[1], "matriz não quadrada"
    assert A.dtype == bool, "matriz deve ser booleana"
    assert not A.diagonal().any(), "laços não são permitidos"
    assert (A == A.T).all(), "matriz não simétrica"
