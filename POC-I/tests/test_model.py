import numpy as np
import pandas as pd
from sklearn.ensemble import HistGradientBoostingClassifier

from poc.graphs import GraphSpec, generate
from poc.heuristics import is_valid_coloring, run_heuristic
from poc.model import (CAND_RAW, CONTEXT, FEATURE_NAMES, candidate_features,
                       global_features, pick_per_step, run_learned_policy, training_table)
from poc.rollout import rollout_trajectory


class FlagModel:
    """"Modelo" que dá probabilidade 1 aos candidatos sugeridos pela heurística h."""

    def __init__(self, h):
        self.col = FEATURE_NAMES.index(f"by_{h}")

    def predict_proba(self, X):
        p = X[:, self.col]
        return np.column_stack([1 - p, p])


def _rollout_table(specs):
    frames, graphs = [], []
    for spec in specs:
        A = generate(spec)
        r = rollout_trajectory(A, seed=7)
        df = pd.DataFrame(r.rows)
        df.insert(0, "graph_id", spec.graph_id)
        frames.append(df)
        graphs.append({"graph_id": spec.graph_id, **global_features(A)})
    return pd.concat(frames, ignore_index=True), pd.DataFrame(graphs)


def test_features_single_step_path_matches_batch():
    """O caminho rápido da política (um passo) calcula os mesmos atributos do treino."""
    R, G = _rollout_table([GraphSpec("er", 40, 0.4, 0)])
    X, _, meta = training_table(R, G, informative_only=False)
    gf = G.iloc[0].to_dict()
    for sid in meta["step_id"].unique()[::5]:
        rows = (meta["step_id"] == sid).to_numpy()
        sub = R.loc[rows]
        Xs = candidate_features({k: sub[k].to_numpy() for k in (*CAND_RAW, *CONTEXT)}, None,
                                sub["suggested_by"].to_numpy(), gf)
        assert np.allclose(Xs, X[rows])
    assert X.shape[1] == len(FEATURE_NAMES)


def test_flag_models_reproduce_heuristics(random_graphs):
    """Seguir sempre o candidato do RLF (ou do DSATUR) reproduz a heurística."""
    for gid, A in random_graphs[:6]:
        for h in ("rlf", "dsatur"):
            r = run_learned_policy(FlagModel(h), A, seed=3)
            ref = run_heuristic(h, A, seed=3)
            assert (r.order == ref.order).all() and r.n_colors == ref.n_colors, (gid, h)
            assert r.follows[h] == A.shape[0]


def test_trained_policy_gives_valid_coloring():
    specs = [GraphSpec("er", 40, p, s) for p in (0.3, 0.6) for s in (0, 1)]
    R, G = _rollout_table(specs)
    X, y, meta = training_table(R, G)
    assert meta["informative"].all() and set(np.unique(y)) <= {0, 1}
    clf = HistGradientBoostingClassifier(max_iter=20, random_state=0).fit(X, y)
    pick = pick_per_step(clf.predict_proba(X)[:, 1], meta["step_id"].to_numpy())
    assert pick.sum() == meta["step_id"].nunique()
    A = generate(GraphSpec("er", 50, 0.5, 9))
    r1 = run_learned_policy(clf, A, seed=1)
    r2 = run_learned_policy(clf, A, seed=1)
    assert is_valid_coloring(A, r1.colors)
    assert (r1.order == r2.order).all()                 # determinística
