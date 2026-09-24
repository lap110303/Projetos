#!/usr/bin/env python3
"""Reproduz os experimentos do artigo: varreduras de ordem n, temperatura tau e
suavização alpha, com N sementes por configuração. Salva results/*.csv e figures/*.pdf."""
import argparse
import csv
import os
import sys

import numpy as np

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))
from bachmarkov.corpus import load_corpus  # noqa: E402
from bachmarkov.evaluate import (CorpusIndex, diversity, parallels_rate,  # noqa: E402
                                 stylistic_distance)
from bachmarkov.generate import generate_chorale  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), "..")


def run_config(order, tau, alpha, mode, seeds, index, ref):
    pieces, rows = [], []
    for s in seeds:
        ev, meta = generate_chorale(order, tau, alpha, mode, seed=s)
        L, src = index.longest_copy(ev)
        rows.append(dict(longest_copy=L, copy_ratio=L / len(ev), source=src,
                         novelty8=index.novelty(ev, 8), n_notes=len(ev)))
        pieces.append(ev)
    res = dict(order=order, tau=tau, alpha=alpha, mode=mode)
    for k in ["longest_copy", "copy_ratio", "novelty8", "n_notes"]:
        v = [r[k] for r in rows]
        res[k], res[k + "_sd"] = float(np.mean(v)), float(np.std(v))
    res["full_copies"] = sum(r["copy_ratio"] >= 0.999 for r in rows)
    res.update(stylistic_distance(pieces, [c["events"] for c in ref]))
    res["diversity"] = diversity(pieces)
    res["parallels"] = parallels_rate(pieces)
    return res


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n-seeds", type=int, default=30)
    ap.add_argument("--mode", default="major")
    a = ap.parse_args()
    seeds = list(range(1000, 1000 + a.n_seeds))
    ref = load_corpus(a.mode)
    index = CorpusIndex(ref)

    base = dict(order=0, tau=0, alpha=0, mode=a.mode)
    corpus_row = {**base, **stylistic_distance([c["events"] for c in ref[: len(ref) // 2]],
                                               [c["events"] for c in ref[len(ref) // 2:]]),
                  "parallels": parallels_rate([c["events"] for c in ref]),
                  "diversity": diversity([c["events"] for c in ref[:30]])}

    sweeps = {
        "order": [(n, 1.0, 0.1) for n in range(1, 7)],
        "tau": [(3, t, 0.1) for t in (0.5, 0.75, 1.0, 1.5, 2.0, 3.0)],
        "alpha": [(3, 1.0, al) for al in (0.01, 0.1, 1.0, 10.0, 100.0)],
    }
    os.makedirs(os.path.join(ROOT, "results"), exist_ok=True)
    all_rows = {}
    for name, cfgs in sweeps.items():
        rows = []
        for n, t, al in cfgs:
            r = run_config(n, t, al, a.mode, seeds, index, ref)
            rows.append(r)
            print(f"[{name}] n={n} tau={t} alpha={al}: copy={r['copy_ratio']:.2f} "
                  f"full={r['full_copies']} nov8={r['novelty8']:.2f} "
                  f"JSint={r['js_interval']:.4f} phr={r['phrase_len']:.1f} "
                  f"div={r['diversity']:.2f} par={r['parallels']:.3f}", flush=True)
        rows.append(corpus_row)
        all_rows[name] = rows
        with open(os.path.join(ROOT, "results", f"sweep_{name}_{a.mode}.csv"), "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=list(rows[0].keys()), extrasaction="ignore")
            w.writeheader()
            w.writerows(rows)
    print("[corpus ref] ", {k: round(v, 4) if isinstance(v, float) else v for k, v in corpus_row.items()})
    from plot_results import plot_all  # noqa: E402
    plot_all(a.mode)


if __name__ == "__main__":
    sys.path.insert(0, os.path.dirname(__file__))
    main()
