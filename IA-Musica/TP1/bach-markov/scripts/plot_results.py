#!/usr/bin/env python3
"""Gera a figura do artigo a partir de results/*.csv."""
import csv
import os

import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402

ROOT = os.path.join(os.path.dirname(__file__), "..")


def load(name, mode):
    with open(os.path.join(ROOT, "results", f"sweep_{name}_{mode}.csv")) as f:
        rows = list(csv.DictReader(f))
    return [r for r in rows if float(r["order"]) > 0], [r for r in rows if float(r["order"]) == 0][0]


def col(rows, k):
    return [float(r[k]) for r in rows]


def plot_all(mode="major"):
    plt.rcParams.update({"font.size": 7.5, "font.family": "serif"})
    fig, axes = plt.subplots(1, 3, figsize=(7.0, 1.95))
    rows, ref = load("order", mode)
    x = [int(r["order"]) for r in rows]
    nseeds = 30

    ax = axes[0]
    ax.errorbar(x, col(rows, "copy_ratio"), col(rows, "copy_ratio_sd"), marker="o", ms=3,
                capsize=2, label="maior cópia / tamanho")
    ax.plot(x, col(rows, "novelty8"), "s--", ms=3, label="8-gramas inéditos")
    ax.plot(x, [v / nseeds for v in col(rows, "full_copies")], "^:", ms=3, label="cópias integrais")
    ax.set_xlabel("ordem $n$")
    ax.set_ylim(-0.05, 1.62)
    ax.set_yticks([0, 0.25, 0.5, 0.75, 1.0])
    ax.set_title("(a) Memorização vs. ordem", fontsize=8)
    ax.legend(frameon=False, fontsize=5.8, loc="upper left", ncol=1, handlelength=2.2, borderaxespad=0.2)
    ax.grid(alpha=0.3)

    ax = axes[1]
    ax.plot(x, col(rows, "phrase_len_sd"), "o-", ms=3, color="C3", label="gerado")
    ax.axhline(float(ref["phrase_len_sd"]), color="k", lw=0.8, ls="--", label="corpus Bach")
    ax.set_xlabel("ordem $n$")
    ax.set_ylabel("DP compr. de frase (notas)")
    ax.set_title("(b) Irregularidade de frase", fontsize=8)
    ax.legend(frameon=False, fontsize=6)
    ax.grid(alpha=0.3)

    rows, ref = load("alpha", mode)
    ax = axes[2]
    ax.semilogx(col(rows, "alpha"), [100 * v for v in col(rows, "parallels")], "o-", ms=3,
                color="C2", label="gerado ($n$=3)")
    ax.axhline(100 * float(ref["parallels"]), color="k", lw=0.8, ls="--", label="corpus Bach")
    ax.set_xlabel(r"suavização harmônica $\alpha$")
    ax.set_ylabel("5as/8as paralelas (%)")
    ax.set_title("(c) Condução de vozes", fontsize=8)
    ax.legend(frameon=False, fontsize=6)
    ax.grid(alpha=0.3)

    fig.tight_layout(w_pad=0.8)
    out = os.path.join(ROOT, "figures", "sweeps.pdf")
    fig.savefig(out, bbox_inches="tight")
    fig.savefig(out.replace(".pdf", ".png"), dpi=200, bbox_inches="tight")
    print("figura salva em", out)


if __name__ == "__main__":
    plot_all()
