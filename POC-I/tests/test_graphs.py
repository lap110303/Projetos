import numpy as np

from poc.graphs import GraphSpec, check_adjacency, generate, num_edges
from poc.grids import ba_m_values, tier_a, tier_b, tier_s


def test_generate_is_valid_and_reproducible():
    for spec in (GraphSpec("er", 200, 0.3, 1), GraphSpec("ba", 200, 7, 1)):
        A1, A2 = generate(spec), generate(spec)
        check_adjacency(A1)
        assert (A1 == A2).all()


def test_different_seeds_differ():
    assert not (generate(GraphSpec("er", 100, 0.5, 0)) ==
                generate(GraphSpec("er", 100, 0.5, 1))).all()


def test_ba_edge_count():
    for n, m in ((100, 2), (300, 30), (1000, 300)):
        assert num_edges(generate(GraphSpec("ba", n, m, 0))) == m * (n - m)


def test_er_density_close_to_p():
    for p in (0.05, 0.5, 0.95):
        A = generate(GraphSpec("er", 600, p, 0))
        assert abs(num_edges(A) / (600 * 599 / 2) - p) < 0.01


def test_tier_sizes():
    assert len(tier_a()) == 2755
    assert len(tier_b()) == 813
    assert len(tier_s()) == 63
    assert len({g.graph_id for g in tier_a()}) == 2755      # ids únicos
    assert ba_m_values(50) == [2, 3, 5, 8, 10, 15]
    assert ba_m_values(1000) == [2, 3, 5, 8, 20, 50, 100, 200, 300]
