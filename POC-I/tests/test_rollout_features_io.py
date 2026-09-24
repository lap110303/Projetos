import networkx as nx
import numpy as np

from poc.features import average_clustering, degeneracy, graph_features, replay
from poc.graphs import GraphSpec, from_networkx, generate, to_networkx
from poc.heuristics import HEURISTICS, is_valid_coloring, run_heuristic
from poc.io_dimacs import read_dimacs, write_dimacs
from poc.rollout import rollout_trajectory
from poc.storage import load_graph, save_graph


def test_rollout_oracle_valid_and_not_worse_than_bases(random_graphs):
    for gid, A in random_graphs[:10]:
        r = rollout_trajectory(A, seed=0, verify=True)   # verifica cache DSATUR
        assert is_valid_coloring(A, r.colors), gid
        # garantia da versão 2: nunca pior que a melhor das duas políticas-base
        base = min(run_heuristic("dsatur", A).n_colors, run_heuristic("rlf", A).n_colors)
        assert r.n_colors <= base, gid
        rows = r.rows
        assert (rows["rollout_colors"] ==
                np.minimum(rows["rollout_dsatur"], rows["rollout_rlf"])).all()
        # o valor do candidato escolhido nunca aumenta ao longo da trajetória
        chosen_vals = rows["rollout_colors"][rows["chosen"] == 1]
        assert (np.diff(chosen_vals) <= 0).all() and chosen_vals[-1] == r.n_colors
        # exatamente um "chosen" por passo, e ele tem rótulo 1
        steps, counts = np.unique(rows["step"][rows["chosen"] == 1], return_counts=True)
        assert len(steps) == A.shape[0] and (counts == 1).all()
        assert (rows["label"][rows["chosen"] == 1] == 1).all()
        # rótulo 1 <=> valor mínimo do passo
        for t in np.unique(rows["step"]):
            sel = rows["step"] == t
            best = rows["rollout_colors"][sel].min()
            assert ((rows["rollout_colors"][sel] == best) == (rows["label"][sel] == 1)).all()


def test_replay_reproduces_every_heuristic(random_graphs):
    """Reexecutar a ordem de qualquer heurística com a regra da menor cor
    reproduz exatamente suas cores (inclusive no RLF)."""
    for gid, A in random_graphs:
        for h in HEURISTICS:
            r = run_heuristic(h, A, seed=2)
            cols, colors = replay(A, r.order)
            assert (colors == r.colors).all(), (gid, h)
            assert cols["n_colors_before"][-1] + cols["opens_new_color"][-1] == r.n_colors
            assert (cols["saturation"] <= cols["colored_nb"]).all()


def test_graph_features_match_networkx(random_graphs):
    for gid, A in random_graphs:
        G = to_networkx(A)
        f = graph_features(A)
        assert f["m"] == G.number_of_edges()
        assert np.isclose(f["avg_clustering"], nx.average_clustering(G), atol=1e-6)
        assert f["degeneracy"] == max(nx.core_number(G).values())


def test_clustering_sampling_is_close():
    A = generate(GraphSpec("er", 800, 0.3, 0))
    exact, _ = average_clustering(A)
    approx, is_exact = average_clustering(A, exact_limit=100, sample=300)
    assert not is_exact and abs(exact - approx) < 0.01


def test_storage_roundtrip(tmp_path):
    for spec in (GraphSpec("er", 300, 0.05, 0), GraphSpec("er", 301, 0.9, 0),
                 GraphSpec("ba", 400, 3, 0)):
        A = generate(spec)
        save_graph(tmp_path / f"{spec.graph_id}.npz", A)
        assert (load_graph(tmp_path / f"{spec.graph_id}.npz") == A).all()


def test_dimacs_roundtrip(tmp_path):
    A = from_networkx(nx.petersen_graph())
    write_dimacs(tmp_path / "p.col", A, "petersen")
    assert (read_dimacs(tmp_path / "p.col") == A).all()
