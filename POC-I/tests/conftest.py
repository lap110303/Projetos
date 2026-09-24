import networkx as nx
import numpy as np
import pytest

from poc.graphs import GraphSpec, from_networkx, generate


def crown_graph(k: int) -> np.ndarray:
    """Grafo coroa: u_i ~ v_j para i != j. Bipartido (χ=2), mas a ordem
    u1, v1, u2, v2, ... leva o guloso a usar k cores."""
    n = 2 * k
    A = np.zeros((n, n), dtype=bool)
    for i in range(k):
        for j in range(k):
            if i != j:
                A[2 * i, 2 * j + 1] = A[2 * j + 1, 2 * i] = True
    return A


@pytest.fixture(scope="session")
def random_graphs():
    """Amostra variada de grafos ER e BA pequenos/médios."""
    specs = [GraphSpec("er", n, p, 0) for n in (30, 80, 150) for p in (0.1, 0.5, 0.9)]
    specs += [GraphSpec("ba", n, m, 0) for n in (30, 80, 150) for m in (2, 5, 20)]
    return [(g.graph_id, generate(g)) for g in specs]


@pytest.fixture(scope="session")
def small_graphs():
    """Grafos pequenos (n <= 12) para comparar com o número cromático exato."""
    out = [(f"er12_{i}", generate(GraphSpec("er", 12, p, i)))
           for i, p in enumerate((0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8))]
    out.append(("petersen", from_networkx(nx.petersen_graph())))
    out.append(("grotzsch", from_networkx(nx.mycielski_graph(4))))
    return out
