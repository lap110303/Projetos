"""Seleciona as peças de demonstração e sintetiza o áudio.

Sem escolha a dedo: pegam-se as N primeiras amostras de avaliação de cada
sistema (ordem da semente) que tenham pelo menos 4 compassos.
Saídas: samples/<sistema>/showcase/{*.mid,*.wav} e samples/showcase.csv
"""
import argparse
import shutil
import sys
from pathlib import Path

import pandas as pd

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.audio import render  # noqa: E402
from chorales.data import ROOT  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=5)
    ap.add_argument("--n_ref", type=int, default=2)
    args = ap.parse_args()
    df = pd.read_csv(ROOT / "results/metrics_per_piece.csv")
    rows = []
    for system in ["transformer", "markov", "test"]:
        n = args.n_ref if system == "test" else args.n
        src_dir = ROOT / ("data/midi/test_ref" if system == "test" else f"samples/{system}/eval")
        out = ROOT / "samples" / ("human" if system == "test" else system) / "showcase"
        if out.exists():
            shutil.rmtree(out)
        out.mkdir(parents=True)
        cand = df[(df.system == system) & (df.n_bars >= 4)].sort_values("file").head(n)
        for _, r in cand.iterrows():
            dst = out / r.file
            shutil.copy(src_dir / r.file, dst)
            engine = render(dst, dst.with_suffix(".wav"))
            rows.append({"system": system, "file": str(dst.relative_to(ROOT)).replace("\\", "/"),
                         "bars": round(r.n_bars, 1), "notes": int(r.n_notes),
                         "pitch_range": int(r.pitch_range), "polyphony": round(r.polyphony, 2),
                         "scale_consistency": round(r.scale_consistency, 3), "audio": engine})
            print(rows[-1])
    pd.DataFrame(rows).to_csv(ROOT / "samples/showcase.csv", index=False)


if __name__ == "__main__":
    main()
