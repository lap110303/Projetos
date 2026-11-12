"""Figuras do artigo: curvas de treino/validação e distribuições das métricas."""
import json
import sys
from pathlib import Path

import matplotlib
import numpy as np
import pandas as pd

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from scipy.stats import gaussian_kde  # noqa: E402

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from chorales.data import ROOT  # noqa: E402
from chorales.metrics import METRICS  # noqa: E402

COLORS = {"test": "#52514e", "transformer": "#2a78d6", "markov": "#eb6834"}
LABELS = {"test": "Humano (teste)", "transformer": "Transformer", "markov": "Markov"}
STYLES = {"test": "-", "transformer": "-", "markov": "--"}
plt.rcParams.update({"font.size": 7, "axes.titlesize": 7, "axes.labelsize": 7,
                     "legend.fontsize": 6.5, "xtick.labelsize": 6, "ytick.labelsize": 6,
                     "axes.spines.top": False, "axes.spines.right": False,
                     "axes.edgecolor": "#888", "axes.grid": True, "grid.color": "#e6e6e6",
                     "grid.linewidth": 0.5, "lines.linewidth": 1.4, "pdf.fonttype": 42})
FIG = ROOT / "figures"


def training_curves():
    hist = json.loads((ROOT / "results/transformer_history.json").read_text())["history"]
    mk = json.loads((ROOT / "results/markov_orders.json").read_text())
    fig, (a, b) = plt.subplots(1, 2, figsize=(3.4, 1.55), sharey=True)
    ep = [h["epoch"] for h in hist]
    a.plot(ep, [h["train_nll"] for h in hist], color=COLORS["transformer"], ls="--", lw=1.2,
           label="treino")
    a.plot(ep, [h["valid_nll"] for h in hist], color=COLORS["transformer"], label="validação")
    best = min(hist, key=lambda h: h["valid_nll"])
    a.plot(best["epoch"], best["valid_nll"], "o", ms=3.5, color=COLORS["transformer"],
           mec="white", mew=0.8)
    alt = ROOT / "results/transformer_history_do015.json"
    if alt.exists():
        h2 = json.loads(alt.read_text())["history"]
        a.plot([h["epoch"] for h in h2], [h["valid_nll"] for h in h2], color="#9cc2ee", lw=1.0,
               label="valid. (dropout 0,15)")
    a.set_xlabel("época")
    a.set_ylabel("NLL (nats/token)")
    a.set_title("(a) Transformer")
    a.legend(frameon=False, loc="lower left", handlelength=1.6)
    c = mk["curve"]
    n = [r["order"] for r in c]
    b.plot(n, [r["valid_nll"] for r in c], color=COLORS["markov"], label="validação")
    b.plot(n, [r["train_nll"] for r in c], color=COLORS["markov"], ls="--", lw=1.2, label="treino")
    b.plot(mk["best_order"], mk["best_valid_nll"], "o", ms=3.5, color=COLORS["markov"],
           mec="white", mew=0.8)
    b.axhline(best["valid_nll"], color=COLORS["transformer"], lw=0.8, ls=":")
    b.set_xlabel("ordem $n$")
    b.set_title("(b) Markov")
    lo = min(min(r["train_nll"] for r in c), min(h["train_nll"] for h in hist))
    b.set_ylim(max(0, lo - 0.1), max(r["valid_nll"] for r in c[1:]) + 0.1)
    fig.tight_layout(pad=0.3, w_pad=0.6)
    fig.savefig(FIG / "training.pdf")
    fig.savefig(FIG / "training.png", dpi=200)


def metric_kdes():
    df = pd.read_csv(ROOT / "results/metrics_per_piece.csv")
    names = {**METRICS, "n_bars": "Nº de compassos"}
    keys = list(names)
    fig, axes = plt.subplots(2, 4, figsize=(7.0, 2.7))
    for ax, k in zip(axes.ravel(), keys):
        allv = df[k].to_numpy(float)
        allv = allv[np.isfinite(allv)]
        lo, hi = np.percentile(allv, [0.5, 99.5])
        pad = 0.08 * (hi - lo + 1e-9)
        grid = np.linspace(lo - pad, hi + pad, 300)
        for s in ["test", "transformer", "markov"]:
            v = df.loc[df.system == s, k].to_numpy(float)
            v = v[np.isfinite(v)]
            if np.ptp(v) < 1e-9:
                v = v + np.random.default_rng(0).normal(0, 1e-3, len(v))
            ax.plot(grid, gaussian_kde(v)(grid), color=COLORS[s], ls=STYLES[s], label=LABELS[s])
            if s == "test":
                ax.fill_between(grid, gaussian_kde(v)(grid), color=COLORS[s], alpha=0.12, lw=0)
        ax.set_title(names[k])
        ax.set_yticks([])
        ax.grid(axis="y", visible=False)
    axes[0, 0].legend(frameon=False, loc="upper left", bbox_to_anchor=(0, -1.55), ncol=3)
    handles, labels = axes[0, 0].get_legend_handles_labels()
    axes[0, 0].get_legend().remove()
    fig.legend(handles, labels, frameon=False, loc="lower center", ncol=3)
    fig.tight_layout(pad=0.3, h_pad=0.6, rect=(0, 0.07, 1, 1))
    fig.savefig(FIG / "metrics.pdf")
    fig.savefig(FIG / "metrics.png", dpi=200)


if __name__ == "__main__":
    FIG.mkdir(exist_ok=True)
    training_curves()
    metric_kdes()
    print("figuras em", FIG)
