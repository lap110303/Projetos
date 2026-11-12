"""Figuras do artigo final (PDF vetorial, larguras de coluna do modelo IEEE).

Saída em docs/relatorio/figs/. Cada figura é gerada só se os dados dela existem:
  vencedora.pdf, rotulo.pdf, passos.pdf, ritmo.pdf   base (data/) e análise exploratória
  generalizacao.pdf                                    results/poc_modelo/tabelas/
  dimacs_espaco.pdf, dimacs_politica.pdf               results/dimacs/tabela_completa.csv

Uso (a partir da pasta POC-I):
    python scripts/paper_figs.py
"""
from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

import matplotlib.pyplot as plt                     # noqa: E402
import numpy as np                                  # noqa: E402
import pandas as pd                                 # noqa: E402
from matplotlib.colors import ListedColormap        # noqa: E402
from matplotlib.ticker import PercentFormatter      # noqa: E402

import eda                                          # noqa: E402
from eda import H, HCOLOR, HNAME, WCATS, WCOLOR     # noqa: E402

OUT = ROOT / "docs" / "relatorio" / "figs"
COL, DCOL = 3.5, 7.16                               # larguras IEEE (polegadas)
POLICY = "#4a3aa7"
plt.rcParams.update({
    "font.family": "serif", "font.serif": ["Times New Roman", "DejaVu Serif"],
    "mathtext.fontset": "stix", "font.size": 8, "axes.titlesize": 8, "axes.labelsize": 8,
    "xtick.labelsize": 7, "ytick.labelsize": 7, "legend.fontsize": 7,
    "figure.facecolor": "white", "axes.facecolor": "white", "savefig.facecolor": "white",
    "axes.titleweight": "normal", "lines.linewidth": 1.4, "pdf.fonttype": 42,
    "grid.linewidth": 0.4, "axes.linewidth": 0.6, "xtick.color": "#333333",
    "ytick.color": "#333333", "axes.labelcolor": "#222222", "text.color": "#111111",
    "axes.edgecolor": "#888888",
})
_BR = plt.FuncFormatter(lambda v, _: f"{v:g}".replace(".", ","))


def br(ax, which="xy"):
    """Vírgula decimal nos eixos indicados."""
    if "x" in which:
        ax.xaxis.set_major_formatter(_BR)
    if "y" in which:
        ax.yaxis.set_major_formatter(_BR)


def save(fig, name):
    OUT.mkdir(parents=True, exist_ok=True)
    fig.savefig(OUT / f"{name}.pdf", bbox_inches="tight", pad_inches=0.02)
    plt.close(fig)
    print("  ", name)


def fig_vencedora(A):
    present = [c for c in WCATS if (A.winner == c).any()]
    codes = {c: i for i, c in enumerate(present)}
    cmap = ListedColormap([WCOLOR[c] for c in present])
    A = A.assign(wcode=A.winner.map(codes).astype(int))
    mode = lambda s: s.value_counts().idxmax()      # noqa: E731
    fig, axes = plt.subplots(1, 2, figsize=(DCOL, 2.3))
    er = A[A.model == "er"].groupby(["param", "n"]).wcode.agg(mode).unstack("n")
    axes[0].imshow(er.to_numpy(), origin="lower", aspect="auto", cmap=cmap, vmin=-0.5,
                   vmax=len(present) - 0.5, interpolation="nearest")
    axes[0].set_xticks(range(len(er.columns))[1::4], [str(c) for c in er.columns][1::4])
    axes[0].set_yticks(range(len(er.index))[1::4],
                       [f"{p:.2f}".replace(".", ",") for p in er.index][1::4])
    axes[0].set(xlabel="$n$", ylabel="$p$", title="(a) Erdős–Rényi")
    axes[0].grid(False)
    ba = A[A.model == "ba"].groupby(["n", "param"]).wcode.agg(mode).reset_index()
    axes[1].scatter(ba.n, ba.param, c=ba.wcode, cmap=cmap, vmin=-0.5, vmax=len(present) - 0.5,
                    s=14, marker="s", linewidths=0)
    axes[1].set_yscale("log")
    axes[1].set(xlabel="$n$", ylabel="$m$ (escala log)", title="(b) Barabási–Albert")
    labels = {"DSATUR": "DSATUR vence", "RLF": "RLF vence"}
    fig.legend([plt.Rectangle((0, 0), 1, 1, color=WCOLOR[c]) for c in present],
               [labels.get(c, "empate DSATUR = RLF") for c in present], loc="lower center",
               ncol=len(present), bbox_to_anchor=(0.5, -0.07), frameon=False)
    fig.tight_layout()
    save(fig, "vencedora")


def fig_rotulo(R, O, gid="er_n150_p0.90_s0"):
    r, o = R[R.graph_id == gid], O[O.graph_id == gid].iloc[0]
    best = r.groupby("step").rollout_colors.min()
    inf = r.groupby("step").label.min() == 0
    fig, ax = plt.subplots(figsize=(COL, 1.9))
    for t in inf[inf].index:
        ax.axvspan(t - 0.5, t + 0.5, color="#ebe8e0", linewidth=0)
    for h, y in (("dsatur", o.colors_dsatur), ("rlf", o.colors_rlf)):
        ax.axhline(y, color=HCOLOR[h], linestyle=":", linewidth=1.2)
        ax.text(len(best) - 1, y + 0.2, f"{HNAME[h]}: {y}", color=HCOLOR[h], ha="right",
                va="bottom", fontsize=7)
    s = r[r.suggested_by.str.contains("rlf", regex=False)].set_index("step").rollout_colors
    ax.plot(s.index, s, color=HCOLOR["rlf"], linewidth=1, alpha=0.8, label="candidato do RLF")
    ax.plot(best.index, best, color="#172033", linewidth=1.6, label="melhor candidato")
    ax.text(len(best) - 1, o.oracle_colors - 0.3, f"oráculo: {o.oracle_colors}", ha="right",
            va="top", fontsize=7)
    ax.set_ylim(o.oracle_colors - 1.8, o.colors_dsatur + 1.3)
    ax.set_xlim(-1, len(best))
    ax.set(xlabel="passo", ylabel="cores ao completar")
    ax.legend(loc="center left", bbox_to_anchor=(0.02, 0.69), frameon=False, labelspacing=0.3)
    save(fig, "rotulo")


def fig_passos(R, O):
    R = R.copy()
    R["key"] = pd.factorize(R.graph_id)[0].astype(np.int64) * 100_000 + R.step
    R["informative"] = R.groupby("key").label.transform("min") == 0
    R = R.merge(O[["graph_id", "model"]], on="graph_id")
    R["phase"] = np.minimum((R.step_frac * 10).astype(int), 9)
    steps = R.drop_duplicates("key")
    fig, axes = plt.subplots(1, 2, figsize=(DCOL, 1.9))
    for fam, col, lab in (("er", "#2a78d6", "Erdős–Rényi"), ("ba", "#104281", "Barabási–Albert")):
        s = steps[steps.model == fam].groupby("phase").informative.mean()
        axes[0].plot((s.index + 0.5) / 10, s, color=col, marker="o", markersize=2.5, label=lab,
                     linestyle="-" if fam == "er" else "--")
    axes[0].set(xlabel="fração dos vértices já coloridos", ylabel="passos informativos",
                title="(a) Fração de passos informativos")
    axes[0].yaxis.set_major_formatter(PercentFormatter(1.0, 0))
    axes[0].set_ylim(0, 1)
    axes[0].legend(frameon=False)
    inf = R[R.informative]
    for h in H:
        s = inf[inf.suggested_by.str.contains(h, regex=False)].groupby("phase").label.mean()
        axes[1].plot((s.index + 0.5) / 10, s, color=HCOLOR[h], marker="o", markersize=2.5,
                     label=HNAME[h])
    axes[1].set(xlabel="fração dos vértices já coloridos", ylabel="candidato ótimo",
                title="(b) Acerto da sugestão de cada heurística")
    axes[1].yaxis.set_major_formatter(PercentFormatter(1.0, 0))
    axes[1].legend(frameon=False, ncol=2)
    for ax in axes:
        br(ax, "x")
    fig.tight_layout()
    save(fig, "passos")


def fig_ritmo(A):
    t = eda.trajectories(ROOT / "data", A).sort_values(["graph_id", "q"])
    fin = t.groupby("graph_id")[list(H)].transform("last")
    frac = t[list(H)] / fin.to_numpy()
    frac[["x", "model"]] = t[["x", "model"]]
    fig, axes = plt.subplots(1, 2, figsize=(COL, 1.7), sharey=True)
    for ax, fam, title in zip(axes, ("er", "ba"), ("(a) ER", "(b) BA")):
        cur = frac[frac.model == fam].groupby("x")[list(H)].mean()
        for h in H:
            ax.plot(cur.index, cur[h], color=HCOLOR[h], label=HNAME[h], linewidth=1.1)
        ax.plot([0, 1], [0, 1], color="#c3c2b7", linewidth=0.6, linestyle=":")
        ax.set(title=title, xlabel="fração colorida")
        ax.yaxis.set_major_formatter(PercentFormatter(1.0, 0))
        br(ax, "x")
    axes[0].set_ylabel("cores já abertas")
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, frameon=False, loc="lower center", ncol=4,
               bbox_to_anchor=(0.5, -0.1), fontsize=6.5)
    fig.tight_layout()
    save(fig, "ritmo")


def fig_generalizacao(G):
    fig, axes = plt.subplots(2, 1, figsize=(COL, 3.0), sharex=True)
    for ax, fam, title in zip(axes, ("er", "ba"), ("(a) Erdős–Rényi", "(b) Barabási–Albert")):
        d = G[G.model == fam]
        for lab, col, ref, ls in (("melhor heurística", POLICY, "best", "-"),
                                  ("RLF", HCOLOR["rlf"], "colors_rlf", "--"),
                                  ("DSATUR", HCOLOR["dsatur"], "colors_dsatur", ":")):
            s = ((d.colors_model - d[ref]) / d[ref]).groupby(d.n).mean()
            ax.plot(s.index, s, color=col, linestyle=ls, label=f"vs {lab}")
        ax.axhline(0, color="#9a978e", linewidth=0.6)
        ax.axvspan(500, 1000, color="#f0efec", zorder=0)
        ax.set_title(title)
        ax.yaxis.set_major_formatter(PercentFormatter(1.0, 0))
    axes[1].set_xlabel("$n$ (faixa cinza: fora da faixa de treino)")
    fig.supylabel("diferença relativa de cores", fontsize=8)
    handles, labels = axes[0].get_legend_handles_labels()
    fig.tight_layout()
    fig.legend(handles, labels, frameon=False, ncol=3, loc="upper center",
               bbox_to_anchor=(0.55, 0.0), fontsize=6.5)
    save(fig, "generalizacao")


def fig_dimacs(D, A):
    fig, ax = plt.subplots(figsize=(COL, 2.3))
    for fam, col, lab in (("er", "#9ec5f4", "treino: ER"), ("ba", "#1c5cab", "treino: BA")):
        s = A[A.model == fam]
        ax.scatter(s.density, s.deg_cv, s=3, alpha=0.5, color=col, label=lab, linewidths=0)
    ax.scatter(D.density, D.deg_std / D.deg_mean, s=10, color="#eb6834", label="DIMACS",
               edgecolors="white", linewidths=0.3)
    ax.set(xscale="log", yscale="log", xlabel="densidade", ylabel="coef. de variação do grau")
    ax.legend(frameon=False, loc="lower left", markerscale=1.5)
    save(fig, "dimacs_espaco")

    stats = D.groupby("familia").d_best.agg(["mean", "size"]).sort_values("mean")
    fig, ax = plt.subplots(figsize=(COL, 0.17 * len(stats) + 0.6))
    for i, f in enumerate(stats.index):
        s = D[D.familia == f].d_best.to_numpy()
        jit = (np.arange(len(s)) - (len(s) - 1) / 2) * (0.5 / max(len(s), 1))
        ax.scatter(s, np.full(len(s), i) + jit, color=POLICY, s=6, alpha=0.8, linewidths=0)
    ax.axvline(0, color="#9a978e", linewidth=0.6)
    ax.set_yticks(range(len(stats)), [f"{f} ({n})" for f, n in stats["size"].items()])
    ax.set_xlabel("cores da política − cores da melhor heurística")
    ax.grid(axis="y", visible=False)
    save(fig, "dimacs_politica")


def main() -> int:
    data = ROOT / "data"
    if (data / "A" / "graphs").exists():
        A = eda.load_tier(data, "A")
        fig_vencedora(A)
        fig_ritmo(A)
        R = pd.read_parquet(data / "B" / "rollouts")
        O = pd.read_parquet(data / "B" / "oracle")
        fig_rotulo(R, O)
        fig_passos(R, O)
    else:
        print("base ausente em data/: figuras da base puladas")
        A = None
    g = ROOT / "results" / "poc_modelo" / "tabelas" / "5_generalizacao_por_grafo.csv"
    if g.exists():
        fig_generalizacao(pd.read_csv(g))
    else:
        print("generalização por grafo ausente (rode scripts/train_poc.py)")
    t = ROOT / "results" / "dimacs" / "tabela_completa.csv"
    if t.exists() and A is not None:
        D = pd.read_csv(t)
        if "deg_std" not in D:
            import json
            extra = pd.DataFrame([json.loads(p.read_text(encoding="utf-8"))
                                  for p in (data / "dimacs" / "resultados").glob("*.json")])
            D = D.merge(extra[["instancia", "deg_std", "deg_mean"]], on="instancia")
        fig_dimacs(D, A)
    else:
        print("resultados do DIMACS ausentes (rode scripts/process_dimacs.py)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
