"""Figuras do pitch (letras grandes, para leitura em vídeo) em docs/pitch/figs/.

Uso (a partir da pasta POC-I):
    python scripts/pitch_figs.py
"""
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]
import matplotlib.pyplot as plt                       # noqa: E402
import numpy as np                                    # noqa: E402
import pandas as pd                                   # noqa: E402
from matplotlib.colors import ListedColormap          # noqa: E402

import eda                                            # noqa: E402
from eda import H, HCOLOR, HNAME, WCATS, WCOLOR       # noqa: E402

OUT = ROOT / "docs" / "pitch" / "figs"
OUT.mkdir(parents=True, exist_ok=True)
BG = "#F7F6F2"
plt.rcParams.update({"font.size": 17, "axes.titlesize": 20, "axes.labelsize": 17,
                     "xtick.labelsize": 15, "ytick.labelsize": 15, "legend.fontsize": 16,
                     "figure.facecolor": BG, "axes.facecolor": BG, "savefig.facecolor": BG})

# ------------------------------------------------------------ 1. quem vence onde
A = eda.load_tier(ROOT / "data", "A")
present = [c for c in WCATS if (A.winner == c).any()]
codes = {c: i for i, c in enumerate(present)}
cmap = ListedColormap([WCOLOR[c] for c in present])
A["wcode"] = A["winner"].map(codes).astype(int)
mode = lambda s: s.value_counts().idxmax()            # noqa: E731
fig, axes = plt.subplots(1, 2, figsize=(16, 6.4))
er = A[A.model == "er"].groupby(["param", "n"])["wcode"].agg(mode).unstack("n")
axes[0].imshow(er.to_numpy(), origin="lower", aspect="auto", cmap=cmap,
               vmin=-0.5, vmax=len(present) - 0.5, interpolation="nearest")
axes[0].set_xticks(range(len(er.columns))[1::4], [str(c) for c in er.columns][1::4])
axes[0].set_yticks(range(len(er.index))[1::4], [f"{p:.2f}".replace(".", ",") for p in er.index][1::4])
axes[0].set_xlabel("n (vértices)")
axes[0].set_ylabel("p (probabilidade de aresta)")
axes[0].set_title("Erdős–Rényi")
axes[0].grid(False)
ba = A[A.model == "ba"].groupby(["n", "param"])["wcode"].agg(mode).reset_index()
axes[1].scatter(ba["n"], ba["param"], c=ba["wcode"], cmap=cmap, vmin=-0.5,
                vmax=len(present) - 0.5, s=110, marker="s", edgecolors=BG, linewidths=0.8)
axes[1].set_yscale("log")
axes[1].set_xlabel("n (vértices)")
axes[1].set_ylabel("m (arestas por vértice novo)")
axes[1].set_title("Barabási–Albert")
handles = [plt.Rectangle((0, 0), 1, 1, color=WCOLOR[c]) for c in present]
labels = [{"DSATUR": "DSATUR vence", "RLF": "RLF vence"}.get(c, "empate") for c in present]
fig.legend(handles, labels, loc="lower center", ncol=len(present), bbox_to_anchor=(0.5, -0.06),
           frameon=False)
fig.tight_layout()
fig.savefig(OUT / "vencedora.png", dpi=150, bbox_inches="tight")
plt.close(fig)

# ------------------------------------------------------------ 2. rótulo por passo (exemplo)
GID = "er_n150_p0.90_s0"
R = pd.read_parquet(ROOT / "data" / "B" / "rollouts", filters=[("graph_id", "==", GID)])
O = pd.read_parquet(ROOT / "data" / "B" / "oracle")
o = O[O.graph_id == GID].iloc[0]
best = R.groupby("step").rollout_colors.min()
inf = R.groupby("step").label.min() == 0
fig, ax = plt.subplots(figsize=(16, 6.2))
for t in inf[inf].index:
    ax.axvspan(t - 0.5, t + 0.5, color="#E9E6DE", linewidth=0)
n = len(best)
for h, y in (("dsatur", o.colors_dsatur), ("rlf", o.colors_rlf)):
    ax.axhline(y, color=HCOLOR[h], linestyle=":", linewidth=2.4)
    ax.text(n - 1, y + 0.25, f"{HNAME[h]} sozinho: {y} cores", color=HCOLOR[h],
            fontsize=17, fontweight="bold", ha="right", va="bottom")
s = R[R.suggested_by.str.contains("rlf", regex=False)].set_index("step").rollout_colors
ax.plot(s.index, s, color=HCOLOR["rlf"], linewidth=2, alpha=0.75,
        label="valor do candidato do RLF")
ax.plot(best.index, best, color="#172033", linewidth=3.4,
        label="valor do melhor candidato (oráculo)")
ax.text(n - 1, o.oracle_colors - 0.35, f"oráculo: {o.oracle_colors} cores", color="#172033",
        fontsize=17, fontweight="bold", ha="right", va="top")
ax.set_ylim(o.oracle_colors - 1.6, o.colors_dsatur + 1.4)
ax.set_xlim(-1, n)
ax.set_xlabel("passo da construção (vértices já coloridos)  ·  faixas sombreadas: passos informativos")
ax.set_ylabel("cores ao completar a coloração")
ax.legend(loc="upper left", frameon=False, ncol=2)
fig.tight_layout()
fig.savefig(OUT / "rotulo_por_passo.png", dpi=150, bbox_inches="tight")
plt.close(fig)
print("ok", inf.mean())
