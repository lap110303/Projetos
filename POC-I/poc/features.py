"""Extração de atributos.

* vertex_features: atributos locais de vértices candidatos em um estado parcial;
* replay: reexecuta uma ordem de coloração e registra os atributos do vértice
  escolhido em cada passo (é assim que geramos a tabela de trajetórias para
  qualquer heurística, sem instrumentar cada uma delas);
* graph_features: atributos globais do grafo.
"""
from __future__ import annotations

import numpy as np

from .state import ColoringState

STEP_FEATURES = (
    "degree",           # grau no grafo original
    "saturation",       # nº de cores distintas na vizinhança
    "uncolored_nb",     # vizinhos ainda sem cor (grau no subgrafo não colorido)
    "colored_nb",       # vizinhos já coloridos
    "avail_color",      # menor cor disponível (cor que o vértice receberia)
    "opens_new_color",  # 1 se avail_color == nº de cores já usadas
    "n_colors_before",  # cores já usadas antes do passo
    "step_frac",        # passo / n
)


def vertex_features(s: ColoringState, vs: np.ndarray) -> dict[str, np.ndarray]:
    """Atributos dos vértices `vs` no estado `s` (antes de colori-los)."""
    vs = np.asarray(vs, dtype=np.int64)
    avail = np.argmin(s.has[:, vs], axis=0).astype(np.int32)
    return {
        "degree": s.deg[vs],
        "saturation": s.sat[vs],
        "uncolored_nb": s.udeg[vs],
        "colored_nb": s.deg[vs] - s.udeg[vs],
        "avail_color": avail,
        "opens_new_color": (avail == s.n_colors).astype(np.int8),
        "n_colors_before": np.full(len(vs), s.n_colors, dtype=np.int32),
        "step_frac": np.full(len(vs), s.step / s.n, dtype=np.float32),
    }


def replay(A: np.ndarray, order: np.ndarray) -> tuple[dict[str, np.ndarray], np.ndarray]:
    """Reexecuta `order` com a regra da menor cor e registra os atributos por passo.

    Retorna (colunas, cores_finais). As colunas incluem "step" e "vertex".
    """
    n = A.shape[0]
    s = ColoringState(A)
    cols = {k: np.empty(n, dtype=np.float32 if k == "step_frac" else np.int32)
            for k in STEP_FEATURES}
    for t, v in enumerate(order):
        v = int(v)
        cols["degree"][t] = s.deg[v]
        cols["saturation"][t] = s.sat[v]
        cols["uncolored_nb"][t] = s.udeg[v]
        cols["colored_nb"][t] = s.deg[v] - s.udeg[v]
        before = s.n_colors
        c = s.assign(v)
        cols["avail_color"][t] = c
        cols["opens_new_color"][t] = int(c == before)
        cols["n_colors_before"][t] = before
        cols["step_frac"][t] = t / n
    cols["step"] = np.arange(n, dtype=np.int32)
    cols["vertex"] = np.asarray(order, dtype=np.int32)
    return cols, s.color.copy()


# --------------------------------------------------------------------------- #
# Atributos globais
# --------------------------------------------------------------------------- #
def _local_clustering_exact(A: np.ndarray) -> np.ndarray:
    A32 = A.astype(np.float32)
    tri2 = ((A32 @ A32) * A32).sum(axis=1)          # 2 * nº de triângulos por vértice
    d = A.sum(axis=1).astype(np.float64)
    with np.errstate(divide="ignore", invalid="ignore"):
        c = np.where(d >= 2, tri2 / (d * (d - 1)), 0.0)
    return c


def average_clustering(A: np.ndarray, sample: int = 500, seed: int = 0,
                       exact_limit: int = 2000) -> tuple[float, bool]:
    """Coeficiente de agrupamento médio. Exato se n <= exact_limit; senão,
    estimado por amostragem de `sample` vértices. Retorna (valor, exato?)."""
    n = A.shape[0]
    if n <= exact_limit:
        return float(_local_clustering_exact(A).mean()), True
    rng = np.random.default_rng(seed)
    vals = []
    for v in rng.choice(n, size=min(sample, n), replace=False):
        nb = np.flatnonzero(A[v])
        d = len(nb)
        if d < 2:
            vals.append(0.0)
            continue
        vals.append(A[np.ix_(nb, nb)].sum() / (d * (d - 1)))
    return float(np.mean(vals)), False


def degeneracy(A: np.ndarray) -> int:
    """Degenerescência (maior k-core), removendo repetidamente o vértice de menor grau."""
    n = A.shape[0]
    d = A.sum(axis=1).astype(np.int64)
    removed = np.zeros(n, dtype=bool)
    big = n + 1
    k = 0
    for _ in range(n):
        v = int(np.argmin(np.where(removed, big, d)))
        k = max(k, int(d[v]))
        removed[v] = True
        d -= A[v]
    return k


def graph_features(A: np.ndarray) -> dict[str, float]:
    n = A.shape[0]
    deg = A.sum(axis=1)
    m = int(deg.sum()) // 2
    clust, exact = average_clustering(A)
    return {
        "n": n,
        "m": m,
        "density": 2 * m / (n * (n - 1)) if n > 1 else 0.0,
        "deg_min": int(deg.min()),
        "deg_max": int(deg.max()),
        "deg_mean": float(deg.mean()),
        "deg_std": float(deg.std()),
        "avg_clustering": clust,
        "clustering_exact": exact,
        "degeneracy": degeneracy(A),
    }
