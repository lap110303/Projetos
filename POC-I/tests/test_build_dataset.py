import sys
from pathlib import Path

import pandas as pd

from poc.graphs import GraphSpec
from poc.heuristics import HEURISTICS
from poc.storage import load_graph

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import build_dataset as bd  # noqa: E402

SPECS = [GraphSpec("er", n, p, s) for n in (20, 30) for p in (0.2, 0.6) for s in (0, 1)]
SPECS += [GraphSpec("ba", 30, m, s) for m in (2, 5) for s in (0, 1)]


def _ids(out, tier, table):
    return pd.read_parquet(out / tier / table)["graph_id"]


def test_build_tier_graph_tables_and_resume(tmp_path):
    out = tmp_path / "data"
    # primeira execução parcial (simula interrupção), lotes de 2 grafos
    s = bd.build_tier("A", SPECS, out, workers=1, batch_graphs=2, limit=5)
    assert s["done"] == 5 and s["missing"] == len(SPECS) - 5
    # retomada: processa só o que falta
    s = bd.build_tier("A", SPECS, out, workers=1, batch_graphs=2)
    assert s["processed_last_run"] == len(SPECS) - 5 and s["missing"] == 0
    # terceira execução não faz nada
    assert bd.build_tier("A", SPECS, out, workers=1)["processed_last_run"] == 0

    g = pd.read_parquet(out / "A" / "graphs")
    assert sorted(g["graph_id"]) == sorted(x.graph_id for x in SPECS)
    runs, steps = _ids(out, "A", "runs"), pd.read_parquet(out / "A" / "steps")
    assert runs.value_counts().eq(len(HEURISTICS)).all()
    n_of = dict(zip(g["graph_id"], g["n"]))
    vc = steps["graph_id"].value_counts()
    assert all(vc[i] == len(HEURISTICS) * n_of[i] for i in n_of)
    for x in SPECS:                         # grafos salvos e recarregáveis
        assert load_graph(out / "graphs" / f"{x.graph_id}.npz").shape == (x.n, x.n)


def test_orphan_parts_are_removed(tmp_path):
    out = tmp_path / "data"
    bd.build_tier("A", SPECS[:3], out, workers=1)
    # lote interrompido: parte 7 gravada em runs/steps, mas não no índice
    for t in ("runs", "steps"):
        src = next((out / "A" / t).glob("part-*.parquet"))
        (out / "A" / t / "part-00007.parquet").write_bytes(src.read_bytes())
    (out / "A" / "graphs" / "part-00008.parquet.tmp").write_bytes(b"lixo")
    bd.build_tier("A", SPECS[:4], out, workers=1)
    for t in ("runs", "steps"):
        assert not (out / "A" / t / "part-00007.parquet").exists()
    assert not list((out / "A" / "graphs").glob("*.tmp"))
    assert _ids(out, "A", "runs").value_counts().eq(len(HEURISTICS)).all()
    assert _ids(out, "A", "graphs").is_unique


def test_rollout_tier_in_parallel(tmp_path):
    out = tmp_path / "data"
    specs = SPECS[:6]
    s = bd.build_tier("B", specs, out, workers=2, batch_graphs=4)
    assert s["missing"] == 0 and not s["failed_last_run"]
    oracle = pd.read_parquet(out / "B" / "oracle")
    roll = pd.read_parquet(out / "B" / "rollouts")
    assert oracle["graph_id"].is_unique and len(oracle) == len(specs)
    assert (oracle["oracle_colors"] <= oracle[["colors_dsatur", "colors_rlf"]].min(axis=1)).all()
    chosen = roll[roll["chosen"] == 1].groupby("graph_id").size()
    assert (chosen == oracle.set_index("graph_id")["n"].reindex(chosen.index)).all()


def test_failures_are_not_written(tmp_path, monkeypatch):
    out = tmp_path / "data"
    orig = bd.work_graph

    def flaky(args):
        if args[0].graph_id == SPECS[1].graph_id:
            return args[0].graph_id, None, "erro simulado"
        return orig(args)

    monkeypatch.setitem(bd.WORKERS, "A", flaky)
    s = bd.build_tier("A", SPECS[:3], out, workers=1)
    assert s["failed_last_run"] == [SPECS[1].graph_id] and s["missing"] == 1
    monkeypatch.setitem(bd.WORKERS, "A", orig)
    s = bd.build_tier("A", SPECS[:3], out, workers=1)     # retomada refaz a falha
    assert s["missing"] == 0 and s["processed_last_run"] == 1
