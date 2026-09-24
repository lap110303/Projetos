import networkx as nx
import numpy as np
import pytest

from poc.exact import chromatic_number
from poc.graphs import from_networkx
from poc.heuristics import (HEURISTICS, StaticOrderPolicy, dsatur_choice,
                            dsatur_complete, is_valid_coloring, rlf, rlf_choice,
                            rlf_complete, run_heuristic, run_policy)
from poc.state import ColoringState
from tests.conftest import crown_graph


# ----------------------------------------------------------------- validade
@pytest.mark.parametrize("h", HEURISTICS)
def test_valid_on_random_graphs(h, random_graphs):
    for gid, A in random_graphs:
        r = run_heuristic(h, A, seed=1)
        assert is_valid_coloring(A, r.colors), gid
        assert r.n_colors == len(np.unique(r.colors)) == r.colors.max() + 1
        assert sorted(r.order.tolist()) == list(range(A.shape[0]))
        assert r.n_colors <= A.sum(axis=1).max() + 1          # cota Δ+1


def test_is_valid_coloring_detects_conflict():
    A = from_networkx(nx.path_graph(3))
    assert is_valid_coloring(A, np.array([0, 1, 0]))
    assert not is_valid_coloring(A, np.array([0, 0, 1]))
    assert not is_valid_coloring(A, np.array([0, 1, -1]))


# ------------------------------------------------------ casos com χ conhecido
@pytest.mark.parametrize("h", HEURISTICS)
def test_trivial_families(h):
    for n in (1, 5, 20):
        E = np.zeros((n, n), dtype=bool)
        assert run_heuristic(h, E).n_colors == 1                  # sem arestas
        K = ~np.eye(n, dtype=bool)
        assert run_heuristic(h, K).n_colors == n                  # completo


@pytest.mark.parametrize("h", HEURISTICS)
def test_never_below_chromatic_number(h, small_graphs):
    for gid, A in small_graphs:
        chi = chromatic_number(A)
        r = run_heuristic(h, A, seed=3)
        assert chi <= r.n_colors, gid


def test_dsatur_exact_on_bipartite_and_cycles():
    # Brélaz (1979): DSATUR é exato em grafos bipartidos.
    for n1, n2, p, seed in ((10, 15, 0.3, 0), (40, 30, 0.2, 1), (60, 60, 0.5, 2)):
        G = nx.bipartite.random_graph(n1, n2, p, seed=seed)
        A = from_networkx(G)
        if A.any():
            assert run_heuristic("dsatur", A).n_colors == 2
    assert run_heuristic("dsatur", crown_graph(10)).n_colors == 2
    for n in (4, 10, 11, 25):
        A = from_networkx(nx.cycle_graph(n))
        assert run_heuristic("dsatur", A).n_colors == (2 if n % 2 == 0 else 3)


def test_order_matters_crown_graph():
    """A ordem u1,v1,u2,v2,... força o guloso a usar k cores num grafo com χ=2."""
    A = crown_graph(8)
    r = run_policy(A, StaticOrderPolicy(np.arange(16)))
    assert r.n_colors == 8


# ------------------------------------------------------- consistência / refs
def test_welsh_powell_matches_networkx_largest_first(random_graphs):
    for gid, A in random_graphs:
        G = nx.Graph()
        G.add_nodes_from(range(A.shape[0]))
        G.add_edges_from(zip(*np.nonzero(np.triu(A, 1))))
        ref = nx.greedy_color(G, strategy="largest_first")
        ours = run_heuristic("welsh_powell", A).colors
        assert all(ours[v] == c for v, c in ref.items()), gid


def test_welsh_powell_bound(random_graphs):
    """Cota de Welsh-Powell: k <= max_i min(d_i + 1, i) com graus decrescentes."""
    for gid, A in random_graphs:
        d = np.sort(A.sum(axis=1))[::-1]
        bound = max(min(d[i] + 1, i + 1) for i in range(len(d)))
        assert run_heuristic("welsh_powell", A).n_colors <= bound, gid


def test_rlf_incremental_equals_stateless_rule(random_graphs):
    """As duas implementações do RLF devem produzir a mesma ordem e cores."""
    for gid, A in random_graphs:
        a = rlf(A)
        b = run_policy(A, rlf_choice, "rlf")
        assert (a.order == b.order).all(), gid
        assert (a.colors == b.colors).all(), gid


def _partial_states(A, rng, n_states=4):
    """Estados parciais "misturados": prefixos de ordens aleatórias e do DSATUR."""
    n = A.shape[0]
    out = []
    for i in range(n_states):
        s = ColoringState(A)
        t = int(rng.integers(0, n))
        perm = rng.permutation(n)
        for j in range(t):
            v = dsatur_choice(s) if (i % 2 and j % 3 == 0) else None
            if v is None:
                v = next(int(u) for u in perm if s.uncolored[u])
            s.assign(v)
        out.append(s)
    return out


def test_rlf_complete_equals_rlf_choice_from_partial_states(random_graphs):
    """A conclusão incremental do RLF (usada nos rollouts) deve coincidir com
    aplicar rlf_choice passo a passo, partindo de estados parciais arbitrários."""
    rng = np.random.default_rng(0)
    for gid, A in random_graphs:
        s0 = ColoringState(A)
        assert rlf_complete(s0) == rlf(A).n_colors and (s0.color == rlf(A).colors).all()
        for s in _partial_states(A, rng):
            a, b = s.copy(), s.copy()
            rlf_complete(a)
            ref = run_policy(A, rlf_choice, state=b)
            assert (a.color == ref.colors).all(), gid
            assert is_valid_coloring(A, a.color), gid


def test_dsatur_complete_from_partial_states(random_graphs):
    rng = np.random.default_rng(1)
    for gid, A in random_graphs:
        for s in _partial_states(A, rng):
            a, b = s.copy(), s.copy()
            dsatur_complete(a)
            ref = run_policy(A, dsatur_choice, state=b)
            assert (a.color == ref.colors).all() and is_valid_coloring(A, a.color), gid


def test_random_reproducible_and_seed_dependent():
    A = from_networkx(nx.gnp_random_graph(100, 0.3, seed=0))
    r1, r2 = run_heuristic("random", A, 5), run_heuristic("random", A, 5)
    r3 = run_heuristic("random", A, 6)
    assert (r1.order == r2.order).all()
    assert not (r1.order == r3.order).all()


def test_state_rejects_invalid_assignments():
    A = from_networkx(nx.path_graph(3))
    s = ColoringState(A)
    s.assign(0)
    with pytest.raises(ValueError):
        s.assign(0)
    with pytest.raises(ValueError):
        s.assign(1, c=0)
