"""Avaliação objetiva: métricas muspy por peça + comparação com o conjunto de teste.

Saídas: results/metrics_per_piece.csv, results/metrics_summary.json
"""
import json
import sys
import warnings
from pathlib import Path

import numpy as np
import pandas as pd

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT  # noqa: E402
from chorales.metrics import METRICS, compare_to_reference, compute_metrics  # noqa: E402
from chorales.tokenizer import load_tokenizer, special_ids, token_types  # noqa: E402

warnings.filterwarnings("ignore")
SYSTEMS = ["transformer", "markov"]
# gramática do REMI usado: quem pode seguir cada tipo de token
NEXT_OK = {"Bar": {"Bar", "Position"}, "Position": {"Pitch"}, "Pitch": {"Duration"},
           "Duration": {"Pitch", "Position", "Bar"}}


def syntax_error_rate(tok, seqs):
    sp = set(special_ids(tok).values())
    errs, n = 0, 0
    for s in seqs:
        types = token_types(tok, [i for i in s if i not in sp])
        if types and types[0] != "Bar":
            errs += 1
        for a, b in zip(types, types[1:]):
            errs += b not in NEXT_OK.get(a, set())
        n += len(types)
    return errs / max(n, 1)


def metrics_for(files, label):
    rows = []
    for f in files:
        try:
            m = compute_metrics(f)
        except Exception as e:      # MIDI vazio/ilegível
            print("  falhou", f.name, e)
            continue
        if m["n_notes"] == 0:
            continue
        rows.append({"system": label, "file": f.name, **m})
    return rows


def main():
    tok = load_tokenizer()
    rows = metrics_for(sorted((ROOT / "data/midi/test_ref").glob("*.mid")), "test")
    for s in SYSTEMS:
        rows += metrics_for(sorted((ROOT / "samples" / s / "eval").glob("*.mid")), s)
    df = pd.DataFrame(rows)
    df.to_csv(ROOT / "results/metrics_per_piece.csv", index=False)

    ref = df[df.system == "test"]
    test_ids = json.loads((ROOT / "data/tokens/test.json").read_text())
    summary = {"test": {"n": len(ref), "syntax_error_rate": syntax_error_rate(tok, test_ids),
                        "metrics": {}}}
    for m in [*METRICS, "n_bars", "n_notes"]:
        v = ref[m].to_numpy(float)
        v = v[np.isfinite(v)]
        intra = np.abs(v[:, None] - v[None, :])[np.triu_indices(len(v), 1)]
        summary["test"]["metrics"][m] = {"mean": float(v.mean()), "std": float(v.std()),
                                         "intra": float(intra.mean())}
    for s in SYSTEMS:
        ids = json.loads((ROOT / "samples" / s / "eval_ids.json").read_text())
        g = df[df.system == s]
        summary[s] = {"n": len(g), "n_generated": len(ids),
                      "syntax_error_rate": syntax_error_rate(tok, ids), "metrics": {}}
        for m in [*METRICS, "n_bars", "n_notes"]:
            gv, rv = g[m].to_numpy(float), ref[m].to_numpy(float)
            summary[s]["metrics"][m] = compare_to_reference(gv[np.isfinite(gv)], rv[np.isfinite(rv)])
    (ROOT / "results/metrics_summary.json").write_text(json.dumps(summary, indent=2))

    # tabela legível
    lines = [f"{'métrica':28s} {'teste':>14s} " + " ".join(f"{s:>30s}" for s in SYSTEMS)]
    for m in [*METRICS, "n_bars"]:
        t = summary["test"]["metrics"][m]
        cells = []
        for s in SYSTEMS:
            r = summary[s]["metrics"][m]
            cells.append(f"{r['mean']:7.3f}±{r['std']:6.3f} OA={r['OA']:.2f} KL={r['KL']:.3f}")
        lines.append(f"{m:28s} {t['mean']:7.3f}±{t['std']:6.3f} " + " ".join(f"{c:>30s}" for c in cells))
    for s in ["test", *SYSTEMS]:
        lines.append(f"{s}: erro sintático = {summary[s]['syntax_error_rate']:.4f}  n = {summary[s]['n']}")
    txt = "\n".join(lines)
    (ROOT / "results/metrics_table.txt").write_text(txt, encoding="utf-8")
    print(txt)


if __name__ == "__main__":
    main()
