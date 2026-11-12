"""Análise exploratória da base (objetivo 5 da proposta).

Lê as tabelas de data/ (camadas A, S e B; o DIMACS NÃO entra) e grava em
results/eda/ as figuras (figs/*.png), as tabelas (tabelas/*.csv) e o relatório
RELATORIO_EDA.md, gerado automaticamente.

Seções do relatório:
  1. Visão geral da base (A e S)
  2. Desempenho das heurísticas na camada A (rótulo por grafo, "opção 1")
  3. Atributos do grafo x heurística vencedora
  4. Escala (camada S)
  5. Trajetórias por passo (camada A)
  6. Rótulos por passo (camada B, "opção 3b")
  7. Conclusões para o modelo

Uso (a partir da pasta POC-I):
    python scripts/eda.py
    python scripts/eda.py --data data --out results/eda
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from pathlib import Path

import matplotlib

if "ipykernel" not in sys.modules:       # no notebook, mantém o backend inline
    matplotlib.use("Agg")
import matplotlib.pyplot as plt                     # noqa: E402
import numpy as np                                  # noqa: E402
import pandas as pd                                 # noqa: E402
from matplotlib.colors import LinearSegmentedColormap, ListedColormap  # noqa: E402

ROOT = Path(__file__).resolve().parents[1]

# --------------------------------------------------------------------------- #
# Convenções visuais (paleta de referência validada; ver skill de visualização)
# --------------------------------------------------------------------------- #
H = ("random", "welsh_powell", "dsatur", "rlf")
HNAME = {"random": "Aleatória", "welsh_powell": "Welsh-Powell",
         "dsatur": "DSATUR", "rlf": "RLF"}
HCOLOR = {"dsatur": "#2a78d6", "rlf": "#eb6834", "welsh_powell": "#1baf7a",
          "random": "#898781"}
ORACLE_COLOR = "#4a3aa7"
# categorias da heurística vencedora (seção 2/3)
WCATS = ("DSATUR", "RLF", "empate DSATUR = RLF", "WP/aleatória")
WCOLOR = {"DSATUR": HCOLOR["dsatur"], "RLF": HCOLOR["rlf"],
          "empate DSATUR = RLF": "#c3c2b7", "WP/aleatória": HCOLOR["welsh_powell"]}
SEQ = ["#cde2fb", "#9ec5f4", "#6da7ec", "#3987e5", "#256abf", "#184f95", "#0d366b"]
SEQ_CMAP = LinearSegmentedColormap.from_list("seq_azul", SEQ)
# divergente: azul = DSATUR melhor, vermelho = RLF melhor, cinza no meio
DIV_CMAP = LinearSegmentedColormap.from_list(
    "div", ["#104281", "#3987e5", "#f0efec", "#e66767", "#a8201f"])
ORD3 = ("#86b6ef", "#2a78d6", "#104281")            # rampa ordinal de 3 passos
INK, INK2, MUTED, GRID = "#0b0b0b", "#52514e", "#898781", "#e1e0d9"

plt.rcParams.update({
    "figure.dpi": 110, "savefig.dpi": 150, "savefig.bbox": "tight",
    "figure.facecolor": "#fcfcfb", "axes.facecolor": "#fcfcfb",
    "font.family": ["Segoe UI", "DejaVu Sans", "sans-serif"], "font.size": 9,
    "text.color": INK, "axes.labelcolor": INK2, "axes.edgecolor": "#c3c2b7",
    "xtick.color": MUTED, "ytick.color": MUTED, "axes.titlesize": 10,
    "axes.titleweight": "bold", "axes.spines.top": False, "axes.spines.right": False,
    "axes.grid": True, "grid.color": GRID, "grid.linewidth": 0.6,
    "lines.linewidth": 2, "legend.frameon": False,
})

FEATURES = ("n", "m", "density", "deg_min", "deg_max", "deg_mean", "deg_std", "deg_cv",
            "deg_max_ratio", "avg_clustering", "clust_ratio", "degeneracy",
            "degen_ratio")
FEATURE_DESC = {
    "n": "nº de vértices", "m": "nº de arestas", "density": "densidade",
    "deg_min": "grau mínimo", "deg_max": "grau máximo", "deg_mean": "grau médio",
    "deg_std": "desvio do grau", "deg_cv": "coef. de variação do grau",
    "deg_max_ratio": "grau máximo / grau médio", "avg_clustering": "agrupamento médio",
    "clust_ratio": "agrupamento / densidade", "degeneracy": "degenerescência",
    "degen_ratio": "degenerescência / grau máximo",
}
STEP_FEATURES = ("degree", "saturation", "uncolored_nb", "colored_nb", "avail_color",
                 "opens_new_color")


# --------------------------------------------------------------------------- #
# Utilitários
# --------------------------------------------------------------------------- #
class Report:
    """Acumula o Markdown, as figuras e as tabelas do relatório.

    inline=True (uso no notebook): exibe cada trecho, figura e tabela na hora, em vez
    de só acumular; com out=None, nada é gravado em disco.
    """

    def __init__(self, out: Path | None, inline: bool = False):
        self.out, self.inline = out, inline
        self.figs = out / "figs" if out else None
        self.tabs = out / "tabelas" if out else None
        for d in (self.figs, self.tabs):
            if d is not None:
                d.mkdir(parents=True, exist_ok=True)
        self.parts: list[str] = []
        self.facts: dict = {}           # números-chave (vão para resumo.json)

    def _show(self, obj) -> None:
        from IPython.display import Markdown, display
        display(Markdown(obj) if isinstance(obj, str) else obj)

    def md(self, text: str) -> None:
        self.parts.append(text.strip("\n") + "\n")
        if self.inline:
            self._show(text)

    def fig(self, fig: plt.Figure, name: str, caption: str) -> None:
        if self.figs is not None:
            fig.savefig(self.figs / f"{name}.png")
        self.parts.append(f"![{caption}](figs/{name}.png)\n\n*{caption}*\n")
        if self.inline:
            self._show(fig)
            self._show(f"*{caption}*")
        plt.close(fig)

    def save_csv(self, df: pd.DataFrame, name: str, index: bool = True) -> None:
        if self.tabs is not None:
            df.to_csv(self.tabs / f"{name}.csv", index=index, encoding="utf-8")

    def table(self, df: pd.DataFrame, name: str, fmt: dict | None = None,
              index: bool = True) -> None:
        self.save_csv(df, name, index)
        self.md(md_table(df, fmt, index))


def md_table(df: pd.DataFrame, fmt: dict | None = None, index: bool = True) -> str:
    fmt = fmt or {}
    d = df.reset_index() if index else df
    cols = list(d.columns)

    def cell(c, v):
        if isinstance(v, str):
            return v
        if v is None or (isinstance(v, (float, np.floating)) and np.isnan(v)):
            return "—"
        if c in fmt:
            return fmt[c](v) if callable(fmt[c]) else num(v, fmt[c])
        if isinstance(v, (float, np.floating)):
            return num(v, ".2f")
        return num(v, ",") if isinstance(v, (int, np.integer)) and abs(v) >= 10_000 else str(v)

    lines = ["| " + " | ".join(map(str, cols)) + " |",
             "|" + "|".join("---" for _ in cols) + "|"]
    for row in d.itertuples(index=False):
        lines.append("| " + " | ".join(cell(c, v) for c, v in zip(cols, row)) + " |")
    return "\n".join(lines) + "\n"


def num(x, spec: str = ".2f") -> str:
    """Formata no padrão brasileiro: vírgula decimal, ponto de milhar."""
    return format(x, spec).translate(str.maketrans(",.", ".,"))


def pct(x: float) -> str:
    return num(100 * x, ".0f") + "%"


def pct1(x: float) -> str:
    return num(100 * x, ".1f") + "%"


def ranges(values: list[float], step: float) -> str:
    """[0.1, 0.15, 0.2, 0.5] -> '0,10–0,20; 0,50' (sequências de passo `step`)."""
    out, run = [], []
    for v in sorted(values):
        if run and not np.isclose(v - run[-1], step):
            out.append(run)
            run = []
        run.append(v)
    if run:
        out.append(run)
    return "; ".join(num(r[0]) if len(r) == 1 else f"{num(r[0])}–{num(r[-1])}" for r in out)


def clusters(pairs: list[tuple[str, str, float]]) -> list[list[str]]:
    """Componentes conexas do grafo de pares redundantes."""
    parent: dict[str, str] = {}

    def find(a):
        parent.setdefault(a, a)
        while parent[a] != a:
            a = parent[a]
        return a

    for a, b, _ in pairs:
        parent[find(a)] = find(b)
    comp: dict[str, list[str]] = {}
    for a in parent:
        comp.setdefault(find(a), []).append(a)
    return [sorted(c, key=FEATURES.index) for c in comp.values()]


def entropy(p: np.ndarray) -> float:
    p = p[p > 0]
    return float(-(p * np.log2(p)).sum())


def mutual_info(x: pd.Series, y: pd.Series, bins: int = 10) -> float:
    """Informação mútua (bits) entre x discretizado em quantis e a categoria y."""
    xb = pd.qcut(x.rank(method="first"), bins, labels=False) if x.nunique() > bins else x
    c = pd.crosstab(xb, y).to_numpy(dtype=float)
    p = c / c.sum()
    px, py = p.sum(axis=1, keepdims=True), p.sum(axis=0, keepdims=True)
    nz = p > 0
    return float((p[nz] * np.log2(p[nz] / (px @ py)[nz])).sum())


def spearman(x: pd.Series, y: pd.Series) -> float:
    """ρ de Spearman = Pearson dos postos (evita depender do scipy)."""
    return float(x.rank().corr(y.rank()))


def add_graph_features(g: pd.DataFrame) -> pd.DataFrame:
    g = g.copy()
    g["deg_cv"] = g["deg_std"] / g["deg_mean"]
    g["deg_max_ratio"] = g["deg_max"] / g["deg_mean"]
    g["clust_ratio"] = g["avg_clustering"] / g["density"]
    g["degen_ratio"] = g["degeneracy"] / g["deg_max"]
    return g


def winner_category(g: pd.DataFrame) -> pd.Series:
    d, r = g["colors_dsatur"], g["colors_rlf"]
    other = g[["colors_welsh_powell", "colors_random"]].min(axis=1)
    cat = np.where(other < np.minimum(d, r), "WP/aleatória",
                   np.where(r < d, "RLF", np.where(d < r, "DSATUR", "empate DSATUR = RLF")))
    return pd.Series(pd.Categorical(cat, categories=WCATS), index=g.index)


def load_tier(data: Path, tier: str) -> pd.DataFrame:
    g = pd.read_parquet(data / tier / "graphs")
    runs = pd.read_parquet(data / tier / "runs")
    wide = runs.pivot(index="graph_id", columns="heuristic", values=["n_colors", "seconds"])
    wide.columns = [f"{'colors' if a == 'n_colors' else 'seconds'}_{h}" for a, h in wide.columns]
    g = g.merge(wide.reset_index(), on="graph_id", validate="one_to_one")
    g = add_graph_features(g)
    g["best"] = g[[f"colors_{h}" for h in H]].min(axis=1)
    for h in H:
        g[f"excess_{h}"] = g[f"colors_{h}"] - g["best"]
        g[f"excess_pct_{h}"] = g[f"excess_{h}"] / g["best"]
    g["diff_dr"] = g["colors_dsatur"] - g["colors_rlf"]          # > 0: RLF melhor
    g["diff_dr_pct"] = g["diff_dr"] / g["best"]
    g["winner"] = winner_category(g)
    g["tier"] = tier
    return g


def model_label(m: str) -> str:
    return {"er": "Erdős–Rényi", "ba": "Barabási–Albert"}[m]


def legend_below(ax, ncol=4, **kw):
    ax.legend(loc="upper center", bbox_to_anchor=(0.5, -0.18), ncol=ncol, **kw)


# --------------------------------------------------------------------------- #
# 1. Visão geral
# --------------------------------------------------------------------------- #
def sec1_overview(rep: Report, A: pd.DataFrame, S: pd.DataFrame, B: pd.DataFrame) -> None:
    rep.md("## 1. Visão geral da base\n")
    both = pd.concat([A, S])
    cnt = (both.groupby(["tier", "model"]).agg(grafos=("graph_id", "size"),
                                               n_min=("n", "min"), n_max=("n", "max"),
                                               arestas_max=("m", "max"),
                                               dens_min=("density", "min"),
                                               dens_max=("density", "max"))
           .rename(index=model_label, level="model"))
    cnt.index.names = ["camada", "modelo"]
    rep.md(f"Camada B: {B['graph_id'].nunique()} grafos (subconjunto de A com n ≤ 500 e "
           f"réplicas 0–2), {num(len(B), ',')} linhas de candidatos.\n")
    rep.table(cnt, "1_contagens", {"arestas_max": ",", "dens_min": ".3f", "dens_max": ".3f"})

    desc = (A.groupby("model")[list(FEATURES)].median().T
            .rename(columns=model_label))
    desc.index = pd.Index([f"{FEATURE_DESC[f]} (`{f}`)" for f in desc.index], name="atributo")
    rep.md("\nMedianas dos atributos globais na camada A:\n")
    rep.table(desc, "1_medianas_atributos",
              {c: (lambda v: num(v, ",.0f") if abs(v) >= 1000 else num(v, ".3g"))
               for c in desc.columns})

    feats = ("density", "deg_cv", "avg_clustering", "clust_ratio", "degen_ratio",
             "deg_max_ratio")
    fig, axes = plt.subplots(2, len(feats), figsize=(13, 4.2), sharey="row")
    for i, model in enumerate(("er", "ba")):
        sub = A[A.model == model]
        for j, f in enumerate(feats):
            ax = axes[i, j]
            ax.hist(sub[f], bins=30, color=SEQ[4], edgecolor="#fcfcfb", linewidth=0.5)
            if i == 1:
                ax.set_xlabel(FEATURE_DESC[f])
            if j == 0:
                ax.set_ylabel(f"{model_label(model)}\nnº de grafos")
    fig.suptitle("Distribuição dos atributos globais na camada A", x=0.01, ha="left",
                 fontweight="bold")
    rep.fig(fig, "1_distribuicoes", "Figura 1 — Distribuição dos atributos globais "
            "(camada A), por modelo gerador.")
    rep.md("Os grafos ER cobrem densidades de 0,05 a 0,95 com grau quase homogêneo "
           "(coeficiente de variação baixo) e agrupamento ≈ densidade (razão ≈ 1). Os BA "
           "são esparsos na maior parte da grade, com grau heterogêneo e agrupamento acima "
           "do esperado para a densidade. As duas famílias ocupam regiões quase disjuntas "
           "do espaço de atributos, o que limita o que se pode concluir sobre grafos "
           "\"intermediários\" (ex.: densos e heterogêneos).\n")


# --------------------------------------------------------------------------- #
# 2. Desempenho das heurísticas (camada A)
# --------------------------------------------------------------------------- #
def _heatmap_er(ax, sub: pd.DataFrame, value: str, cmap, vmin, vmax, agg="mean"):
    piv = sub.pivot_table(index="param", columns="n", values=value, aggfunc=agg)
    im = ax.imshow(piv.to_numpy(dtype=float), origin="lower", aspect="auto", cmap=cmap,
                   vmin=vmin, vmax=vmax, interpolation="nearest")
    ax.set_xticks(range(len(piv.columns))[1::2], [str(c) for c in piv.columns][1::2],
                  rotation=90)
    ax.set_yticks(range(len(piv.index))[1::2], [f"{p:.2f}" for p in piv.index][1::2])
    ax.set_xlabel("n")
    ax.set_ylabel("p")
    ax.grid(False)
    return im


def sec2_heuristics(rep: Report, A: pd.DataFrame) -> None:
    rep.md("## 2. Desempenho das heurísticas (camada A, rótulo por grafo)\n")
    rows = []
    for h in H:
        c = A[f"colors_{h}"]
        is_best = c == A["best"]
        n_best = (A[[f"colors_{k}" for k in H]].to_numpy() == A["best"].to_numpy()[:, None]).sum(1)
        row = {"heurística": HNAME[h]}
        for lab, mask in (("geral", slice(None)), ("ER", A.model == "er"),
                          ("BA", A.model == "ba")):
            row[f"melhor c/ empate ({lab})"] = is_best[mask].mean()
            row[f"melhor sozinha ({lab})"] = (is_best & (n_best == 1))[mask].mean()
        row["cores acima da melhor (média)"] = A[f"excess_{h}"].mean()
        row["% acima da melhor (média)"] = A[f"excess_pct_{h}"].mean()
        row["% acima da melhor (máx.)"] = A[f"excess_pct_{h}"].max()
        rows.append(row)
    tab = pd.DataFrame(rows).set_index("heurística")
    fmt = {c: pct for c in tab.columns if c.startswith("melhor")}
    fmt.update({"% acima da melhor (média)": pct1, "% acima da melhor (máx.)": pct1})
    rep.table(tab, "2_desempenho_heuristicas", fmt)
    rep.facts["A_best_share"] = {h: float((A[f"colors_{h}"] == A["best"]).mean()) for h in H}

    wc = pd.crosstab(A["winner"], A["model"].map(model_label), normalize="columns")
    wc["geral"] = A["winner"].value_counts(normalize=True).reindex(wc.index)
    wc.index.name = "vencedora (categoria)"
    n_other = int((A["winner"] == "WP/aleatória").sum())
    rep.md("\nCategoria vencedora por grafo. \"WP/aleatória\" = Welsh-Powell ou ordem "
           "aleatória estritamente melhor que DSATUR e RLF"
           + (f" ({n_other} grafos)" if n_other else
              " — **não ocorre em nenhum grafo**: a disputa é sempre entre DSATUR e RLF")
           + ".\n")
    present = [c for c in WCATS if (A["winner"] == c).any()]
    rep.table(wc, "2_categoria_vencedora", {c: pct for c in wc.columns})
    rep.facts["A_winner_share"] = {k: float(v) for k, v in wc["geral"].items()}

    # --- mapas: ER em (n, p); BA em (n, m)
    codes = {c: i for i, c in enumerate(WCATS)}
    wcmap = ListedColormap([WCOLOR[c] for c in WCATS])
    er, ba = A[A.model == "er"].copy(), A[A.model == "ba"].copy()
    lim = float(np.nanpercentile(np.abs(A["diff_dr_pct"]), 98)) or 0.01
    fig, axes = plt.subplots(2, 2, figsize=(12, 8.6))
    # moda da categoria nas 5 réplicas
    for df in (er, ba):
        df["wcode"] = df["winner"].map(codes).astype(int)
    mode = er.groupby(["param", "n"])["wcode"].agg(lambda s: s.value_counts().idxmax())
    _heatmap_er(axes[0, 0], mode.reset_index(), "wcode", wcmap, -0.5, len(WCATS) - 0.5)
    axes[0, 0].set_title("ER — categoria vencedora (moda das 5 réplicas)")
    im = _heatmap_er(axes[0, 1], er, "diff_dr_pct", DIV_CMAP, -lim, lim)
    axes[0, 1].set_title("ER — (cores DSATUR − cores RLF) / melhor")
    cb = fig.colorbar(im, ax=axes[0, 1], fraction=0.04)
    cb.ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    cb.set_label("vermelho: RLF melhor · azul: DSATUR melhor")
    bam = ba.groupby(["n", "param"]).agg(wcode=("wcode", lambda s: s.value_counts().idxmax()),
                                         diff=("diff_dr_pct", "mean")).reset_index()
    for ax, col, cmap, vmin, vmax, title in (
            (axes[1, 0], "wcode", wcmap, -0.5, len(WCATS) - 0.5,
             "BA — categoria vencedora (moda das 5 réplicas)"),
            (axes[1, 1], "diff", DIV_CMAP, -lim, lim, "BA — (cores DSATUR − cores RLF) / melhor")):
        sc = ax.scatter(bam["n"], bam["param"], c=bam[col], cmap=cmap, vmin=vmin, vmax=vmax,
                        s=46, marker="s", edgecolors="#fcfcfb", linewidths=0.6)
        ax.set_yscale("log")
        ax.set_xlabel("n")
        ax.set_ylabel("m (arestas por novo vértice, escala log)")
        ax.set_title(title)
    cb = fig.colorbar(sc, ax=axes[1, 1], fraction=0.04)
    cb.ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    cb.set_label("vermelho: RLF melhor · azul: DSATUR melhor")
    handles = [plt.Rectangle((0, 0), 1, 1, color=WCOLOR[c]) for c in present]
    axes[1, 0].legend(handles, present, loc="upper center", bbox_to_anchor=(0.5, -0.14),
                      ncol=len(present))
    fig.tight_layout()
    rep.fig(fig, "2_mapas_vencedora", "Figura 2 — Heurística vencedora e diferença "
            "relativa DSATUR − RLF no espaço de parâmetros da camada A.")

    # texto: onde o RLF vence
    er_p = er.groupby("param").agg(rlf=("winner", lambda s: (s == "RLF").mean()),
                                   dsatur=("winner", lambda s: (s == "DSATUR").mean()),
                                   diff=("diff_dr_pct", "mean"))
    p_rlf = er_p.index[er_p["rlf"] >= 0.5]
    rep.facts["ER_p_rlf_majority"] = [float(p) for p in p_rlf]
    ba_small = ba[ba["param"] <= 8]
    ba_large = ba[ba["param"] > 8]
    rep.md(
        f"No ER, o RLF é estritamente melhor que o DSATUR na maioria das réplicas para "
        f"p em {ranges(list(p_rlf), 0.05) or '—'}; os empates concentram-se em n pequeno "
        f"ou p pequeno, onde as colorações usam poucas cores. "
        f"No BA com m ≤ 8, DSATUR e RLF empatam em "
        f"{pct((ba_small.winner == 'empate DSATUR = RLF').mean())} dos grafos; "
        f"com m > 8, o RLF vence sozinho em {pct((ba_large.winner == 'RLF').mean())} e o "
        f"DSATUR em {pct((ba_large.winner == 'DSATUR').mean())}.\n")

    # --- distribuição do excesso e tempo
    fig, axes = plt.subplots(1, 3, figsize=(13, 3.6))
    for ax, model in zip(axes[:2], ("er", "ba")):
        sub = A[A.model == model]
        data = [100 * sub[f"excess_pct_{h}"] for h in H]
        bp = ax.boxplot(data, orientation="horizontal", widths=0.55, patch_artist=True, showfliers=False,
                        medianprops={"color": INK, "linewidth": 1.5})
        for patch, h in zip(bp["boxes"], H):
            patch.set_facecolor(HCOLOR[h])
            patch.set_edgecolor(HCOLOR[h])
        ax.set_yticks(range(1, len(H) + 1), [HNAME[h] for h in H])
        ax.set_xlabel("% de cores acima da melhor heurística")
        ax.set_title(model_label(model))
    ax = axes[2]
    med = A.groupby("n")[[f"seconds_{h}" for h in H]].median()
    for h in H:
        ax.plot(med.index, med[f"seconds_{h}"], color=HCOLOR[h], label=HNAME[h])
    ax.set_yscale("log")
    ax.set_xlabel("n")
    ax.set_ylabel("tempo mediano (s, escala log)")
    ax.set_title("Tempo de execução")
    legend_below(ax, ncol=2)
    fig.tight_layout()
    rep.fig(fig, "2_excesso_e_tempo", "Figura 3 — Excesso de cores em relação à melhor "
            "heurística (caixas sem pontos extremos) e tempo mediano por n.")


# --------------------------------------------------------------------------- #
# 3. Atributos do grafo x vencedora
# --------------------------------------------------------------------------- #
def sec3_features(rep: Report, A: pd.DataFrame) -> None:
    rep.md("## 3. Atributos do grafo e heurística vencedora\n")
    rows = []
    hy = {lab: entropy(A.loc[mask, "winner"].value_counts(normalize=True).to_numpy())
          for lab, mask in (("geral", A.index), ("ER", A.index[A.model == "er"]),
                            ("BA", A.index[A.model == "ba"]))}
    for f in FEATURES:
        row = {"atributo": f"{FEATURE_DESC[f]} (`{f}`)"}
        for lab, sub in (("geral", A), ("ER", A[A.model == "er"]), ("BA", A[A.model == "ba"])):
            row[f"IM {lab}"] = (mutual_info(sub[f], sub["winner"]) / hy[lab]
                                if hy[lab] > 0 else float("nan"))   # vencedora constante
        row["ρ Spearman c/ (D−R)/melhor"] = spearman(A[f], A["diff_dr_pct"])
        rows.append(row)
    tab = pd.DataFrame(rows).set_index("atributo").sort_values("IM geral", ascending=False)
    rep.md("Informação mútua (IM) entre cada atributo, discretizado em 10 quantis, e a "
           "categoria vencedora, **normalizada pela entropia da categoria** (0 = nenhuma "
           "informação; 1 = determina a vencedora). A última coluna é a correlação de "
           "Spearman com a diferença relativa DSATUR − RLF (positiva: RLF melhor).\n")
    rep.table(tab, "3_informacao_mutua", {c: ".2f" for c in tab.columns})

    # blocos de atributos redundantes e atributos de maior IM, um por bloco
    corr = A[list(FEATURES)].corr(method="spearman")
    pairs = [(a, b, corr.at[a, b]) for i, a in enumerate(FEATURES)
             for b in FEATURES[i + 1:] if abs(corr.at[a, b]) >= 0.95]
    blocks = clusters(pairs)
    block_of = {f: tuple(c) for c in blocks for f in c}
    top, seen = [], set()
    for t in tab.index:
        f = t.split(" (`")[1].rstrip("`)")
        key = block_of.get(f, (f,))
        if key not in seen:
            seen.add(key)
            top.append(f)
    rep.facts["top_mi_features"] = top[:3]
    rep.facts["redundant_blocks"] = blocks
    rep.facts["mi_by_model_top"] = {
        lab: (tab[f"IM {lab}"].idxmax().split(" (`")[1].rstrip("`)")
              if tab[f"IM {lab}"].notna().any() else "—") for lab in ("ER", "BA")}

    # taxa de cada categoria por faixa de atributo
    feats = ("density", "deg_cv", "degen_ratio", "n")
    fig, axes = plt.subplots(1, len(feats), figsize=(13, 3.5), sharey=True)
    for ax, f in zip(axes, feats):
        b = pd.qcut(A[f].rank(method="first"), 10, labels=False)
        share = pd.crosstab(b, A["winner"], normalize="index")
        x = A.groupby(b)[f].median()
        for c in WCATS:
            if c in share:
                ax.plot(x, share[c], color=WCOLOR[c], marker="o", markersize=4, label=c)
        ax.set_xlabel(FEATURE_DESC[f] + " (mediana do decil)")
        ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    axes[0].set_ylabel("fração dos grafos")
    handles, labels = axes[0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncol=4, bbox_to_anchor=(0.5, -0.08))
    fig.suptitle("Categoria vencedora por decil de atributo (camada A)", x=0.01, ha="left",
                 fontweight="bold")
    fig.tight_layout()
    rep.fig(fig, "3_vencedora_por_decil", "Figura 4 — Fração de grafos em cada categoria "
            "vencedora, por decil de quatro atributos globais.")

    # correlação entre atributos
    fig, ax = plt.subplots(figsize=(7.2, 6))
    im = ax.imshow(corr.to_numpy(), cmap=DIV_CMAP.reversed(), vmin=-1, vmax=1)
    ax.set_xticks(range(len(FEATURES)), FEATURES, rotation=90)
    ax.set_yticks(range(len(FEATURES)), FEATURES)
    ax.grid(False)
    for i in range(len(FEATURES)):
        for j in range(len(FEATURES)):
            v = corr.iat[i, j]
            ax.text(j, i, f"{v:.1f}", ha="center", va="center", fontsize=6.5,
                    color="#ffffff" if abs(v) > 0.6 else INK2)
    fig.colorbar(im, ax=ax, fraction=0.04, label="ρ de Spearman")
    ax.set_title("Correlação entre atributos globais (camada A)")
    rep.fig(fig, "3_correlacao_atributos", "Figura 5 — Correlação de Spearman entre os "
            "atributos globais.")
    rep.save_csv(corr, "3_correlacao_atributos")
    if blocks:
        rep.md("Blocos de atributos quase redundantes (ligados por |ρ| ≥ 0,95): " +
               "; ".join("{" + ", ".join(f"`{f}`" for f in c) + "}" for c in blocks) +
               ". Dentro de cada bloco, um atributo basta. Parte da redundância vem da "
               "própria grade: no ER, agrupamento ≈ densidade por construção; e "
               "`deg_cv`, `deg_max_ratio` e `degen_ratio` separam principalmente ER de BA.\n")


# --------------------------------------------------------------------------- #
# 4. Escala (camada S)
# --------------------------------------------------------------------------- #
def sec4_scale(rep: Report, A: pd.DataFrame, S: pd.DataFrame) -> None:
    rep.md("## 4. Escala (camada S)\n")
    params = {"er": sorted(S.loc[S.model == "er", "param"].unique()),
              "ba": sorted(S.loc[S.model == "ba", "param"].unique())}
    ref = A[(A.n.isin([500, 1000])) &
            (((A.model == "er") & A.param.isin(params["er"])) |
             ((A.model == "ba") & A.param.isin(params["ba"])))]
    both = pd.concat([ref, S])
    tab = (both.groupby(["model", "param", "n"])
           .agg(grafos=("graph_id", "size"),
                **{f"cores {HNAME[h]}": (f"colors_{h}", "mean") for h in H},
                **{"RLF < DSATUR": ("diff_dr", lambda s: (s > 0).mean()),
                   "DSATUR < RLF": ("diff_dr", lambda s: (s < 0).mean()),
                   "(D−R)/melhor": ("diff_dr_pct", "mean")}))
    tab = tab.rename(index=model_label, level="model")
    tab.index.names = ["modelo", "parâmetro", "n"]
    fmt = {f"cores {HNAME[h]}": ".1f" for h in H}
    fmt.update({"RLF < DSATUR": pct, "DSATUR < RLF": pct, "(D−R)/melhor": pct1})
    rep.md("Médias por configuração; as linhas com n = 500 e 1.000 vêm da camada A "
           "(5 réplicas), as demais da camada S (3 réplicas).\n")
    rep.table(tab, "4_escala", fmt)
    s_er = S[S.model == "er"]
    rep.facts["S_er_rlf_better"] = float((s_er.diff_dr > 0).mean())
    same = ((s_er.diff_dr > 0).mean() > 0.5) == ((A[A.model == "er"].diff_dr > 0).mean() > 0.5) \
        and ((S[S.model == "ba"].diff_dr < 0).mean() > 0.5) == \
        ((A[A.model == "ba"].diff_dr < 0).mean() > 0.5)
    rep.md(f"Na camada S, o RLF é estritamente melhor que o DSATUR em "
           f"{pct((s_er.diff_dr > 0).mean())} dos grafos ER e em "
           f"{pct((S[S.model == 'ba'].diff_dr > 0).mean())} dos BA. O tempo mediano do "
           f"RLF em n = 10.000 foi de {num(S.loc[S.n == 10000, 'seconds_rlf'].median(), '.1f')} s, "
           f"contra {num(S.loc[S.n == 10000, 'seconds_dsatur'].median(), '.1f')} s do DSATUR. "
           + ("O padrão da camada A se mantém em escala (mesma heurística à frente em cada "
              "modelo).\n" if same else "**O padrão da camada A não se mantém em escala** "
              "(ver tabela).\n"))


# --------------------------------------------------------------------------- #
# 5. Trajetórias por passo (camada A)
# --------------------------------------------------------------------------- #
NQ = 50


def trajectories(data: Path, A: pd.DataFrame) -> pd.DataFrame:
    """Cores acumuladas ao fim de cada 1/NQ da construção, por (grafo, heurística)."""
    st = pd.read_parquet(data / "A" / "steps",
                         columns=["graph_id", "heuristic", "step_frac", "n_colors_before",
                                  "opens_new_color"])
    st["cum"] = st["n_colors_before"] + st["opens_new_color"]
    st["q"] = np.minimum((st["step_frac"].to_numpy() * NQ).astype(np.int16), NQ - 1)
    t = (st.groupby(["graph_id", "q", "heuristic"], observed=True)["cum"].max()
         .unstack("heuristic").reset_index())
    del st
    t = t.merge(A[["graph_id", "model", "n", "param", "best", "density"]], on="graph_id")
    t["x"] = (t["q"] + 1) / NQ
    return t


def sec5_trajectories(rep: Report, data: Path, A: pd.DataFrame) -> None:
    rep.md("## 5. Trajetórias por passo (camada A)\n")
    t = trajectories(data, A)
    groups = [("er", 500, 0.1), ("er", 500, 0.5), ("er", 500, 0.9),
              ("ba", 500, 2), ("ba", 500, 25), ("ba", 500, 150)]
    fig, axes = plt.subplots(2, 3, figsize=(12, 6.4), sharex=True)
    for ax, (model, n, p) in zip(axes.flat, groups):
        sub = t[(t.model == model) & (t.n == n) & np.isclose(t.param, p)]
        if sub.empty:
            ax.set_visible(False)
            continue
        cur = sub.groupby("x")[[*H]].mean().div(sub.groupby("x")["best"].mean(), axis=0)
        for h in H:
            ax.plot(cur.index, cur[h], color=HCOLOR[h], label=HNAME[h])
        ax.set_title(f"{model.upper()}, n = {n}, {'p' if model == 'er' else 'm'} = {p:g}")
        ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    for ax in axes[1]:
        ax.set_xlabel("fração dos vértices já coloridos")
    for ax in axes[:, 0]:
        ax.set_ylabel("cores usadas / cores da melhor")
    handles, labels = axes[0, 0].get_legend_handles_labels()
    fig.legend(handles, labels, loc="lower center", ncol=4, bbox_to_anchor=(0.5, -0.04))
    fig.tight_layout()
    rep.fig(fig, "5_trajetorias", "Figura 6 — Cores acumuladas ao longo da construção, "
            "relativas ao total da melhor heurística (média das 5 réplicas).")

    # ritmo de abertura de cores: fração das cores finais já abertas em x da construção
    marks = (0.1, 0.25, 0.5)
    t = t.sort_values(["graph_id", "q"])
    fin = t.groupby("graph_id")[list(H)].last()
    rows = []
    for x in marks:
        q = int(round(x * NQ)) - 1        # último intervalo com dados até a marca x
        cur = t[t.q <= q].groupby("graph_id")[list(H)].last()
        frac = (cur / fin.reindex(cur.index)).join(A.set_index("graph_id")["model"])
        for model, sub in frac.groupby("model"):
            for h in H:
                rows.append({"heurística": HNAME[h], "modelo": model.upper(),
                             "x": x, "v": sub[h].mean()})
    pace = (pd.DataFrame(rows)
            .pivot_table(index="heurística", columns=["modelo", "x"], values="v")
            .reindex([HNAME[h] for h in H]))
    pace.columns = [f"{m} {pct(x)}" for m, x in pace.columns]
    rep.md("Fração das cores finais de cada heurística já abertas quando 10%, 25% e 50% "
           "dos vértices estão coloridos (média na camada A):\n")
    rep.table(pace, "5_ritmo_abertura_cores", {c: pct for c in pace.columns})
    rep.facts["pace"] = {c: {k: float(v) for k, v in pace[c].items()} for c in pace.columns}
    rlf_half = float(pace.loc["RLF", "ER 50%"])
    ds_half = float(pace.loc["DSATUR", "ER 50%"])
    rep.md(
        f"As heurísticas constroem a coloração de formas estruturalmente diferentes. O "
        f"DSATUR e o Welsh-Powell abrem a maior parte das cores cedo (no ER, o DSATUR já "
        f"abriu {pct(ds_half)} das suas cores na metade da construção) e depois reutilizam; "
        f"o RLF fecha uma classe de cor por vez, então abre cores em ritmo quase constante "
        f"({pct(rlf_half)} na metade). Consequências: (i) comparar \"cores usadas até o "
        f"passo t\" entre heurísticas não indica quem está à frente — a diferença final só "
        f"se revela no fim; (ii) os atributos por passo (`n_colors_before`, `step_frac`, "
        f"`avail_color`) têm distribuições diferentes conforme a política que gerou o "
        f"estado. Um modelo treinado em estados de uma política verá estados diferentes ao "
        f"seguir a própria política (mudança de distribuição), o que precisa ser "
        f"considerado na prova de conceito.\n")


# --------------------------------------------------------------------------- #
# 6. Rótulos por passo (camada B)
# --------------------------------------------------------------------------- #
def sec6_rollouts(rep: Report, data: Path, A: pd.DataFrame) -> dict:
    rep.md("## 6. Rótulos por passo (camada B)\n")
    rep.md("As linhas da camada B seguem a **trajetória do oráculo**: os estados vistos "
           "são os que o oráculo visita, não os de cada heurística isolada.\n")
    R = pd.read_parquet(data / "B" / "rollouts")
    O = pd.read_parquet(data / "B" / "oracle")
    R["key"] = pd.factorize(R["graph_id"])[0].astype(np.int64) * 100_000 + R["step"]
    grp = R.groupby("key")
    R["informative"] = grp["label"].transform("min") == 0
    R["spread"] = grp["rollout_colors"].transform("max") - grp["rollout_colors"].transform("min")
    steps = R.drop_duplicates("key")[["key", "graph_id", "step", "n_candidates",
                                      "informative", "spread", "step_frac"]]
    info_share = float(steps["informative"].mean())
    nc = steps["n_candidates"].value_counts(normalize=True).sort_index()
    rep.md(f"Passos: {num(len(steps), ',')}. Candidatos distintos por passo: " +
           ", ".join(f"{k} = {pct1(v)}" for k, v in nc.items()) +
           f". **Passos informativos** (ao menos um candidato com rótulo 0): "
           f"**{pct1(info_share)}**. Nos demais, todas as sugestões levam ao mesmo valor "
           f"e o passo não ensina nada ao modelo.\n")
    lab_all = float(R["label"].mean())
    lab_inf = float(R.loc[R["informative"], "label"].mean())
    rep.md(f"Balanço das classes: rótulo 1 em {pct1(lab_all)} de todas as linhas e em "
           f"{pct1(lab_inf)} das linhas dos passos informativos. Nos passos informativos, "
           f"a diferença entre o melhor e o pior candidato é de "
           f"{num(steps.loc[steps.informative, 'spread'].mean())} cores em média "
           f"(mediana {steps.loc[steps.informative, 'spread'].median():.0f}).\n")
    rep.facts.update(B_steps=int(len(steps)), B_informative_share=info_share,
                     B_label1_all=lab_all, B_label1_informative=lab_inf)

    # informatividade por modelo e por fase
    steps = steps.merge(O[["graph_id", "model", "n", "param"]], on="graph_id")
    steps["phase"] = np.minimum((steps["step_frac"] * 10).astype(int), 9)
    by_model = steps.groupby("model")["informative"].agg(["mean", "sum"])
    rep.facts["B_informative_by_model"] = {m: float(v) for m, v in by_model["mean"].items()}
    rep.md(f"Por modelo: {pct1(by_model.at['er', 'mean'])} dos passos são informativos no ER "
           f"e {pct1(by_model.at['ba', 'mean'])} no BA — os passos informativos são "
           f"{pct(by_model.at['er', 'sum'] / by_model['sum'].sum())} ER. No BA quase todas as "
           f"sugestões levam ao mesmo número de cores (o DSATUR já é, em geral, ótimo ou "
           f"quase), então o sinal de treino por passo vem essencialmente do ER.\n")
    ch = R[R["chosen"] == 1]
    follow = {lab: pd.Series({HNAME[h]: sub["suggested_by"].str.contains(h, regex=False).mean()
                              for h in H})
              for lab, sub in (("todos", ch), ("informativos", ch[ch.informative]),
                               ("triviais", ch[~ch.informative]))}
    rep.facts["B_oracle_follows"] = {k: {h: float(x) for h, x in v.items()}
                                     for k, v in follow.items()}
    rep.md("Com que frequência o candidato escolhido pelo oráculo era a sugestão de cada "
           "heurística (sugestões coincidentes contam para todas): " +
           "; ".join(f"**{lab}** — " + ", ".join(f"{k} {pct(v)}" for k, v in s.items())
                     for lab, s in follow.items()) +
           ". Nos passos triviais o desempate (DSATUR > RLF > WP > aleatória) faz o oráculo "
           "seguir o DSATUR; nos informativos ele segue sobretudo o RLF. Os estados da "
           "camada B são, portanto, uma mistura dos dois estilos de construção.\n")

    # acerto de cada heurística (candidato sugerido tem rótulo 1)
    hit = {}
    for h in H:
        sub = R[R["suggested_by"].str.contains(h, regex=False)]
        hit[h] = sub.set_index("key")["label"]
    hits = pd.DataFrame(hit)
    hits = hits.join(steps.set_index("key")[["informative", "phase", "model"]])
    inf = hits[hits.informative]
    uniform = float(R[R.informative].groupby("key")["label"].mean().mean())
    tab = pd.DataFrame({
        "acerta (todos os passos)": hits[list(H)].mean(),
        "acerta (informativos)": inf[list(H)].mean(),
        "acerta (inform., ER)": inf.loc[inf.model == "er", list(H)].mean(),
        "acerta (inform., BA)": inf.loc[inf.model == "ba", list(H)].mean(),
        "única a acertar (inform.)": pd.Series(
            {h: ((inf[h] == 1) & (inf[[k for k in H if k != h]].sum(axis=1) == 0)).mean()
             for h in H}),
    })
    tab.index = [HNAME[h] for h in H]
    tab.index.name = "candidato de"
    rep.md("\nFração dos passos em que o candidato sugerido por cada heurística é ótimo "
           f"(rótulo 1). Referência: escolher um candidato distinto ao acaso acerta "
           f"{pct1(uniform)} dos passos informativos.\n")
    rep.table(tab, "6_acerto_heuristicas", {c: pct1 for c in tab.columns})
    rep.facts["B_hit_informative"] = {h: float(inf[h].mean()) for h in H}
    rep.facts["B_uniform_informative"] = uniform

    fig, axes = plt.subplots(1, 2, figsize=(12, 3.8))
    ax = axes[0]
    for model, col, lab in (("er", ORD3[1], "Erdős–Rényi"), ("ba", ORD3[2], "Barabási–Albert")):
        s = steps[steps.model == model].groupby("phase")["informative"].mean()
        ax.plot((s.index + 0.5) / 10, s, color=col, marker="o", markersize=4, label=lab)
    ax.set_title("Passos informativos por fase da construção")
    ax.set_xlabel("fração dos vértices já coloridos")
    ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    ax.set_ylim(0, None)
    legend_below(ax, ncol=2)
    ax = axes[1]
    for h in H:
        s = inf.groupby("phase")[h].mean()
        ax.plot((s.index + 0.5) / 10, s, color=HCOLOR[h], marker="o", markersize=4,
                label=HNAME[h])
    ax.set_title("Acerto do candidato de cada heurística (passos informativos)")
    ax.set_xlabel("fração dos vértices já coloridos")
    ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    legend_below(ax, ncol=4)
    fig.tight_layout()
    rep.fig(fig, "6_informatividade_acerto", "Figura 8 — Fração de passos informativos e "
            "taxa de acerto das sugestões de cada heurística ao longo da construção.")

    # regras de um atributo: escolher o candidato de maior/menor valor
    RI = R[R.informative].copy()
    RI = RI.merge(O[["graph_id", "model"]], on="graph_id")
    g = RI.groupby("key")
    step_model = g["model"].first()
    per_step = {}
    for f in STEP_FEATURES:
        for how in ("max", "min"):
            ext = g[f].transform(how)
            per_step[f"{how}({f})"] = RI[RI[f] == ext].groupby("key")["label"].mean()
    per_step["candidato ao acaso"] = g["label"].mean()
    ps = pd.DataFrame(per_step)
    ps = ps.join(inf[list(H)].rename(columns={h: f"seguir {HNAME[h]}" for h in H}))
    rules = pd.DataFrame({"acerto (geral)": ps.mean(),
                          "acerto (ER)": ps[step_model == "er"].mean(),
                          "acerto (BA)": ps[step_model == "ba"].mean()})
    rules = rules.sort_values("acerto (geral)", ascending=False)
    rules.index.name = "regra"
    rep.md("\nRegras de um único atributo nos passos informativos: escolher o candidato de "
           "maior (`max`) ou menor (`min`) valor; empates entre candidatos contam como "
           "sorteio (acerto esperado = fração de rótulos 1 entre os empatados).\n")
    rep.table(rules, "6_regras_um_atributo", {c: pct1 for c in rules.columns})
    rep.facts["B_best_rule"] = [rules.index[0], float(rules.iloc[0, 0])]

    # diferença pareada de atributos (rótulo 1 − rótulo 0) dentro do passo
    mean1 = RI[RI.label == 1].groupby("key")[list(STEP_FEATURES)].mean()
    mean0 = RI[RI.label == 0].groupby("key")[list(STEP_FEATURES)].mean()
    d = (mean1 - mean0).dropna()
    pair = pd.DataFrame({"diferença média": d.mean(), "rótulo 1 maior": (d > 0).mean(),
                         "iguais": (d == 0).mean(), "rótulo 1 menor": (d < 0).mean()})
    pair.index.name = "atributo"
    rep.md("\nDentro de cada passo informativo: média do atributo nos candidatos ótimos "
           "menos a média nos não ótimos.\n")
    rep.table(pair, "6_diferenca_pareada",
              {"diferença média": ".2f", "rótulo 1 maior": pct, "iguais": pct,
               "rótulo 1 menor": pct})

    # ganho do oráculo
    O["best_h"] = O[[f"colors_{h}" for h in H]].min(axis=1)
    O["gain"] = O["best_h"] - O["oracle_colors"]
    O["gain_pct"] = O["gain"] / O["best_h"]
    O["gain_dr"] = O[["colors_dsatur", "colors_rlf"]].min(axis=1) - O["oracle_colors"]
    gt = (O.groupby(O["model"].map(model_label))
          .agg(grafos=("graph_id", "size"), **{"melhora": ("gain", lambda s: (s > 0).mean())},
               **{"ganho médio (cores)": ("gain", "mean"), "ganho máx. (cores)": ("gain", "max"),
                  "ganho médio (%)": ("gain_pct", "mean"),
                  "tempo médio (s)": ("seconds", "mean")}))
    gt.index.name = "modelo"
    rep.md("\nGanho do oráculo sobre a melhor das 4 heurísticas:\n")
    rep.table(gt, "6_ganho_oraculo", {"melhora": pct, "ganho médio (%)": pct1,
                                      "ganho máx. (cores)": "d"})
    rep.facts["B_oracle_better_share"] = float((O.gain > 0).mean())
    rep.facts["B_oracle_worse_than_min_dr"] = int((O.gain_dr < 0).sum())

    fig, axes = plt.subplots(1, 2, figsize=(12, 4.2))
    er, ba = O[O.model == "er"], O[O.model == "ba"]
    vmax = float(O["gain_pct"].max()) or 0.01
    im = _heatmap_er(axes[0], er, "gain_pct", SEQ_CMAP, 0, vmax)
    axes[0].set_title("ER — ganho relativo do oráculo")
    sc = axes[1].scatter(ba["n"], ba["param"], c=ba["gain_pct"], cmap=SEQ_CMAP, vmin=0,
                         vmax=vmax, s=46, marker="s", edgecolors="#fcfcfb", linewidths=0.6)
    axes[1].set_yscale("log")
    axes[1].set_xlabel("n")
    axes[1].set_ylabel("m (escala log)")
    axes[1].set_title("BA — ganho relativo do oráculo")
    cb = fig.colorbar(sc, ax=axes, fraction=0.03)
    cb.ax.yaxis.set_major_formatter(matplotlib.ticker.PercentFormatter(1.0, 0))
    cb.set_label("(melhor heurística − oráculo) / melhor heurística")
    rep.fig(fig, "6_ganho_oraculo", "Figura 9 — Ganho relativo do oráculo sobre a melhor "
            "heurística, por região da camada B (média das réplicas).")
    hit_ba = inf.loc[inf.model == "ba", list(H)].mean()
    return {"info_share": info_share, "rules": rules, "hit_inf": inf[list(H)].mean(),
            "uniform": uniform, "pair": pair, "best_h_ba": HNAME[hit_ba.idxmax()],
            "best_h_ba_acc": float(hit_ba.max())}


# --------------------------------------------------------------------------- #
# 7. Conclusões
# --------------------------------------------------------------------------- #
def sec7_conclusions(rep: Report, A: pd.DataFrame, b: dict) -> None:
    f = rep.facts
    rules = b["rules"]
    feat_rules = rules[~rules.index.str.startswith(("seguir", "candidato"))]
    best_rule, best_acc = feat_rules.index[0], float(feat_rules.iloc[0, 0])
    best_h = b["hit_inf"].idxmax()
    blocks = f.get("redundant_blocks", [])
    inf_m = f["B_informative_by_model"]
    rep.md("## 7. Conclusões para o modelo (sugestões, a discutir)\n")
    items = [
        f"**A disputa é entre DSATUR e RLF, e a família do grafo quase a decide.** Welsh-Powell "
        f"e ordem aleatória nunca são estritamente as melhores. No ER o RLF vence (ou empata) "
        f"quase sempre; no BA, o DSATUR. Os atributos globais mais informativos (um por bloco "
        f"redundante) são {', '.join(f'`{x}`' for x in f['top_mi_features'])}, mas eles "
        f"separam sobretudo ER de BA: dentro de cada família, os mais informativos são "
        f"`{f['mi_by_model_top']['ER']}` (ER) e `{f['mi_by_model_top']['BA']}` (BA). Um "
        f"classificador por grafo (opção 1) tende a aprender a família geradora; ele deve ser "
        f"avaliado também separadamente por modelo, e o teste no DIMACS dirá se generaliza.",
        "**Atributos redundantes.** " + (
            "Blocos com |ρ| ≥ 0,95: " +
            "; ".join("{" + ", ".join(f"`{x}`" for x in c) + "}" for c in blocks) +
            ". Basta um atributo por bloco." if blocks
            else "Nenhum par de atributos globais com |ρ| ≥ 0,95."),
        f"**Rótulo por passo (opção 3b): treinar só nos passos informativos.** Apenas "
        f"{pct1(b['info_share'])} dos passos da camada B são informativos "
        f"({pct1(inf_m['er'])} no ER, {pct1(inf_m['ba'])} no BA). Treinar em todos inflaria "
        f"a acurácia com exemplos triviais; sugere-se treinar e avaliar a classificação "
        f"apenas nos passos informativos e reportar à parte a fração de triviais. O sinal "
        f"de treino por passo vem quase todo do ER.",
        f"**Linha de base a superar.** Nos passos informativos, seguir o {HNAME[best_h]} "
        f"acerta {pct1(float(b['hit_inf'][best_h]))} (no BA, onde há poucos passos "
        f"informativos, a melhor sugestão é a do {b['best_h_ba']}, com "
        f"{pct1(b['best_h_ba_acc'])}); a melhor regra de um atributo "
        f"(`{best_rule}`) acerta {pct1(best_acc)}; ao acaso, {pct1(b['uniform'])}. A "
        f"regra `min(avail_color)` é essencialmente o princípio do RLF (completar a classe "
        f"de cor atual antes de abrir outra), o que explica seu bom desempenho. O modelo "
        f"precisa superar o candidato do RLF — e, sobretudo, usar menos cores ao **seguir a "
        f"própria política** de ponta a ponta, que é a métrica do objetivo geral.",
        "**Mudança de distribuição.** Os estados da camada B vêm da trajetória do oráculo "
        "(uma mistura de passos no estilo DSATUR e no estilo RLF), e cada política produz "
        "estados com distribuições diferentes (seção 5). Ao seguir a própria política, o modelo verá "
        "estados que não estavam no treino. Para a prova de conceito isso é aceitável, "
        "mas deve ser medido (acerto por passo versus cores finais) e registrado como "
        "limitação a tratar no POC II (ex.: gerar rótulos também nos estados visitados pelo "
        "próprio modelo).",
        "**Formulação sugerida para a prova de conceito:** classificação binária de "
        "candidatos (rótulo 3b) nos passos informativos, com atributos do candidato "
        "(tabela da seção 6), atributos relativos ao passo (ex.: `avail_color` comparado ao "
        "nº de cores já usadas; posição de cada atributo entre os candidatos do passo) e "
        "atributos globais do grafo (um por bloco); escolha do candidato de maior "
        "probabilidade; divisão treino/validação por grafo, estratificada por modelo e n; "
        "avaliação por acerto nos passos informativos e por nº de cores da coloração "
        "completa obtida com a política aprendida, comparado ao DSATUR, ao RLF e ao oráculo.",
    ]
    rep.md("\n".join(f"{i}. {t}" for i, t in enumerate(items, 1)) + "\n")


# --------------------------------------------------------------------------- #
def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Análise exploratória da base.")
    ap.add_argument("--data", type=Path, default=ROOT / "data")
    ap.add_argument("--out", type=Path, default=ROOT / "results" / "eda")
    args = ap.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    t0 = time.perf_counter()
    rep = Report(args.out)

    A = load_tier(args.data, "A")
    S = load_tier(args.data, "S")
    B = pd.read_parquet(args.data / "B" / "rollouts", columns=["graph_id"])
    rep.md("# Análise exploratória da base\n\n"
           "Gerado automaticamente por `scripts/eda.py` a partir de `data/` (camadas A, S e "
           "B). O conjunto DIMACS não é usado aqui: é reservado ao teste final.\n")
    for name, fn in (("1", lambda: sec1_overview(rep, A, S, B)),
                     ("2", lambda: sec2_heuristics(rep, A)),
                     ("3", lambda: sec3_features(rep, A)),
                     ("4", lambda: sec4_scale(rep, A, S)),
                     ("5", lambda: sec5_trajectories(rep, args.data, A))):
        t = time.perf_counter()
        fn()
        print(f"seção {name} concluída em {time.perf_counter() - t:.1f} s", flush=True)
    del B
    t = time.perf_counter()
    b = sec6_rollouts(rep, args.data, A)
    print(f"seção 6 concluída em {time.perf_counter() - t:.1f} s", flush=True)
    sec7_conclusions(rep, A, b)

    (args.out / "RELATORIO_EDA.md").write_text("\n".join(rep.parts), encoding="utf-8")
    (args.out / "resumo.json").write_text(
        json.dumps(rep.facts, indent=2, ensure_ascii=False, default=str), encoding="utf-8")
    print(f"relatório em {args.out / 'RELATORIO_EDA.md'} "
          f"({time.perf_counter() - t0:.0f} s)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
