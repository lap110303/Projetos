import sys
from pathlib import Path

from poc.graphs import GraphSpec

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "scripts"))
import build_dataset as bd  # noqa: E402
import eda  # noqa: E402


def test_eda_runs_on_tiny_dataset(tmp_path):
    """Gera uma base minúscula com o build_dataset e roda a análise completa."""
    data, out = tmp_path / "data", tmp_path / "eda"
    specs_a = [GraphSpec("er", n, p, s) for n in (20, 30) for p in (0.2, 0.6) for s in (0, 1)]
    specs_a += [GraphSpec("ba", n, m, s) for n in (20, 30) for m in (2, 5) for s in (0, 1)]
    specs_s = [GraphSpec("er", 40, 0.2, 0), GraphSpec("ba", 40, 2, 0)]
    bd.build_tier("A", specs_a, data, workers=1)
    bd.build_tier("S", specs_s, data, workers=1)
    bd.build_tier("B", [g for g in specs_a if g.seed == 0], data, workers=1)

    assert eda.main(["--data", str(data), "--out", str(out)]) == 0
    report = (out / "RELATORIO_EDA.md").read_text(encoding="utf-8")
    for sec in range(1, 8):
        assert f"## {sec}." in report
    assert (out / "resumo.json").exists()
    assert len(list((out / "figs").glob("*.png"))) >= 7
