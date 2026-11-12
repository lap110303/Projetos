"""Teste do pipeline do DIMACS sobre uma cópia fictícia (nunca sobre o teste real)."""
import json
import sys
from pathlib import Path

import networkx as nx
import pandas as pd
import pytest

from poc.graphs import GraphSpec, from_networkx, generate
from poc.io_dimacs import write_dimacs, write_dimacs_binary

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / "scripts"))
import process_dimacs as pdx  # noqa: E402

pytestmark = pytest.mark.skipif(not pdx.MODEL_PATH.exists(),
                                reason="modelo da prova de conceito não treinado")


def test_pipeline_on_fake_instances(tmp_path):
    dim = tmp_path / "dimacs"
    (dim / "instancias").mkdir(parents=True)
    graphs = {"petersen": (from_networkx(nx.petersen_graph()), 3.0, "col"),
              "er40": (generate(GraphSpec("er", 40, 0.3, 0)), float("nan"), "col.b"),
              "ba60": (generate(GraphSpec("ba", 60, 3, 0)), float("nan"), "col")}
    rows = []
    for name, (A, chi, ext) in graphs.items():
        f = f"{name}.{ext}"
        (write_dimacs_binary if ext == "col.b" else write_dimacs)(dim / "instancias" / f, A)
        rows.append({"instancia": name, "arquivo": f, "url": "", "familia": "teste",
                     "fonte": "", "chi_conhecido": chi, "bytes": 1, "sha256": ""})
    pd.DataFrame(rows).to_csv(dim / "catalogo.csv", index=False)

    out = tmp_path / "rel"
    assert pdx.main(["--dimacs-dir", str(dim), "--out", str(out), "--workers", "1",
                     "--oracle-max-n", "45"]) == 0
    res = {p.stem: json.loads(p.read_text(encoding="utf-8"))
           for p in (dim / "resultados").glob("*.json")}
    assert set(res) == set(graphs)
    assert res["petersen"]["colors_policy"] >= 3 and "oracle_colors" in res["petersen"]
    assert "oracle_colors" not in res["ba60"]                 # n > --oracle-max-n
    assert len({r["model_sha256"] for r in res.values()}) == 1
    assert (out / "RELATORIO_DIMACS.md").exists() and (out / "tabela_completa.csv").exists()

    # uso único: uma segunda execução não reprocessa nem altera os resultados
    before = {k: v["executado_em"] for k, v in res.items()}
    assert pdx.main(["--dimacs-dir", str(dim), "--out", str(out), "--workers", "1"]) == 0
    after = {p.stem: json.loads(p.read_text(encoding="utf-8"))["executado_em"]
             for p in (dim / "resultados").glob("*.json")}
    assert after == before
