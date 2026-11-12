"""Tabelas do artigo final (LaTeX), geradas a partir dos resultados — sem transcrição manual.

Saída em docs/relatorio/tabelas/:
  heuristicas.tex     desempenho das heurísticas na camada A
  passo.tex           acerto por passo na validação (modelos e linhas de base)
  ponta.tex           política aprendida de ponta a ponta (validação B e réplica 3)
  dimacs_resumo.tex   teste final no DIMACS (política × referências; distância a χ)
  dimacs_completa.tex tabela completa por instância (apêndice, longtable)

Uso (a partir da pasta POC-I):
    python scripts/paper_tables.py
"""
from __future__ import annotations

import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

import numpy as np            # noqa: E402
import pandas as pd           # noqa: E402

import eda                    # noqa: E402
from eda import H, HNAME      # noqa: E402

OUT = ROOT / "docs" / "relatorio" / "tabelas"


def f(x, dec=1, sign=False):
    """Número em LaTeX com vírgula decimal."""
    if x is None or (isinstance(x, float) and np.isnan(x)):
        return "--"
    s = f"{x:+.{dec}f}" if sign else f"{x:.{dec}f}"
    return s.replace("-", "$-$").replace(".", "{,}")


def p(x, dec=0):
    return "--" if x is None or (isinstance(x, float) and np.isnan(x)) else f(100 * x, dec) + r"\%"


def tex_escape(s: str) -> str:
    return s.replace("_", r"\_").replace("&", r"\&").replace("%", r"\%")


def write(name: str, body: str) -> None:
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / f"{name}.tex").write_text(body, encoding="utf-8")
    print("  ", name)


def tab_heuristicas() -> None:
    A = eda.load_tier(ROOT / "data", "A")
    nbest = (A[[f"colors_{h}" for h in H]].to_numpy() == A.best.to_numpy()[:, None]).sum(1)
    lines = []
    for h in H:
        best = A[f"colors_{h}"] == A.best
        alone = best & (nbest == 1)
        cells = [HNAME[h]]
        for fam in ("er", "ba"):
            m = A.model == fam
            cells += [p(best[m].mean()), p(alone[m].mean())]
        cells.append(p(A[f"excess_pct_{h}"].mean(), 1))
        lines.append(" & ".join(cells) + r" \\")
    write("heuristicas", "\n".join([
        r"\begin{tabular}{@{}lccccc@{}}", r"\toprule",
        r" & \multicolumn{2}{c}{ER (1.900)} & \multicolumn{2}{c}{BA (855)} & Excesso \\",
        r"\cmidrule(lr){2-3}\cmidrule(lr){4-5}",
        r"Heurística & melhor & sozinha & melhor & sozinha & médio \\", r"\midrule",
        *lines, r"\bottomrule", r"\end{tabular}"]) + "\n")


def tab_passo() -> None:
    s = json.loads((ROOT / "results" / "poc_modelo" / "resumo.json").read_text(encoding="utf-8"))
    acc = s["step_accuracy"]
    names = {"Gradient boosting": "Gradient boosting", "Regressão logística": "Regressão logística",
             "min(avail_color), desempate por prioridade": "Menor cor disponível",
             "seguir RLF": "Seguir o RLF", "candidato ao acaso": "Candidato ao acaso",
             "seguir DSATUR": "Seguir o DSATUR", "seguir Welsh-Powell": "Seguir o Welsh-Powell",
             "seguir Aleatória": "Seguir a ordem aleatória"}
    lines = []
    for k, lab in names.items():
        a = acc[k]
        bold = k == "Gradient boosting"
        cells = [lab, p(a["acerto geral"], 1), p(a["acerto ER"], 1), p(a["acerto BA"], 1)]
        if bold:
            cells = [rf"\textbf{{{c}}}" for c in cells]
        lines.append(" & ".join(cells) + r" \\")
        if k == "Regressão logística":
            lines.append(r"\midrule")
    write("passo", "\n".join([
        r"\begin{tabular}{@{}lccc@{}}", r"\toprule",
        r"Método & Geral & ER & BA \\", r"\midrule", *lines, r"\bottomrule",
        r"\end{tabular}"]) + "\n")


def tab_ponta() -> None:
    t = ROOT / "results" / "poc_modelo" / "tabelas"
    gb = pd.read_csv(t / "4_politica_por_grafo_gb.csv")
    G = pd.read_csv(t / "5_generalizacao_por_grafo.csv")

    def row(lab, d, best_col, n_col=None):
        dh = d.colors_model - d[best_col]
        return (f"{lab} & {len(d)} & {p((dh < 0).mean())} & {p((dh == 0).mean())} & "
                f"{p((dh > 0).mean())} & {f(dh.mean(), 2, sign=True)} \\\\")

    lines = [r"\multicolumn{6}{@{}l}{\emph{Validação (camada B)}} \\",
             row("ER", gb[gb.model == "er"], "best_h"), row("BA", gb[gb.model == "ba"], "best_h"),
             row("Total", gb, "best_h"), r"\midrule",
             r"\multicolumn{6}{@{}l}{\emph{Réplica 3 da camada A (nunca vista)}} \\",
             row(r"ER, $n \le 500$", G[(G.model == "er") & (G.n <= 500)], "best"),
             row(r"ER, $n > 500$", G[(G.model == "er") & (G.n > 500)], "best"),
             row(r"BA, $n \le 500$", G[(G.model == "ba") & (G.n <= 500)], "best"),
             row(r"BA, $n > 500$", G[(G.model == "ba") & (G.n > 500)], "best"),
             row("Total", G, "best")]
    write("ponta", "\n".join([
        r"\begin{tabular}{@{}lrcccc@{}}", r"\toprule",
        r"Grafos & $N$ & $<$ & $=$ & $>$ & $\bar{\Delta}$ \\", r"\midrule",
        *lines, r"\bottomrule", r"\end{tabular}"]) + "\n")


def tab_dimacs() -> None:
    d = pd.read_csv(ROOT / "results" / "dimacs" / "tabela_completa.csv")
    d["best_h"] = d[[f"colors_{h}" for h in H]].min(axis=1)
    refs = [("Melhor heurística", "best_h"), ("RLF", "colors_rlf"), ("DSATUR", "colors_dsatur")]
    if d.oracle_colors.notna().any():
        refs.append((r"Oráculo ($n \le 500$)", "oracle_colors"))
    lines = []
    for lab, c in refs:
        m = d[c].notna()
        dd = d.colors_policy[m] - d[c][m]
        lines.append(f"{lab} & {int(m.sum())} & {p((dd < 0).mean())} & {p((dd == 0).mean())} & "
                     f"{p((dd > 0).mean())} & {f(dd.mean(), 2, sign=True)} \\\\")
    k = d[d.chi_conhecido.notna()]
    opt = []
    for lab, c in [(HNAME[h], f"colors_{h}") for h in H] + [("Política aprendida", "colors_policy")]:
        opt.append(f"{lab} & {p((k[c] == k.chi_conhecido).mean())} & "
                   f"{p((k[c] / k.chi_conhecido - 1).mean(), 1)} \\\\")
    write("dimacs_resumo", "\n".join([
        r"\begin{tabular}{@{}lrcccc@{}}", r"\toprule",
        r"Política $\times$ & $N$ & $<$ & $=$ & $>$ & $\bar{\Delta}$ \\", r"\midrule",
        *lines, r"\bottomrule", r"\end{tabular}"]) + "\n")
    write("dimacs_otimo", "\n".join([
        r"\begin{tabular}{@{}lcc@{}}", r"\toprule",
        rf"Método ({len(k)} instâncias com $\chi$) & Atinge $\chi$ & Excesso médio \\",
        r"\midrule", *opt, r"\bottomrule", r"\end{tabular}"]) + "\n")

    rows = []
    for fam, g in d.sort_values(["familia", "n", "instancia"]).groupby("familia", sort=True):
        rows.append(rf"\multicolumn{{11}}{{@{{}}l}}{{\emph{{{tex_escape(fam)}}}}} \\")
        for _, r in g.iterrows():
            vals = [r[f"colors_{h}"] for h in H]
            best = min(vals)
            cells = [tex_escape(r.instancia), f"{int(r.n)}", f"{int(r.m)}",
                     "--" if pd.isna(r.chi_conhecido) else f"{int(r.chi_conhecido)}"]
            cells += [rf"\textbf{{{v}}}" if v == best else f"{v}" for v in vals]
            cells.append("--" if pd.isna(r.oracle_colors) else f"{int(r.oracle_colors)}")
            pc = int(r.colors_policy)
            cells.append(rf"\textbf{{{pc}}}" if pc < best else
                         (f"{pc}" if pc == best else rf"\textit{{{pc}}}"))
            cells.append(f(r.seconds_policy, 1))
            rows.append(" & ".join(cells) + r" \\")
    head = (r"Instância & $n$ & $m$ & $\chi$ & Al. & WP & DS & RLF & Or. & Pol. & $t$ (s) \\")
    write("dimacs_completa", "\n".join([
        r"{\scriptsize", r"\setlength{\tabcolsep}{4pt}",
        r"\begin{longtable}{@{}lrrrrrrrrrr@{}}",
        r"\caption{Resultados por instância do DIMACS. Al.\ = ordem aleatória, WP = Welsh-Powell, "
        r"DS = DSATUR, Or.\ = oráculo por rollout ($n \le 500$), Pol.\ = política aprendida; "
        r"$t$ = tempo da política. Em negrito, a melhor heurística e a política quando usa menos "
        r"cores que ela; em itálico, a política quando usa mais.}\label{tab:dimacs_completa}\\",
        r"\toprule", head, r"\midrule", r"\endfirsthead",
        r"\toprule", head, r"\midrule", r"\endhead",
        r"\bottomrule", r"\endlastfoot",
        *rows, r"\end{longtable}", "}"]) + "\n")


def main() -> int:
    tab_heuristicas()
    tab_passo()
    if (ROOT / "results" / "poc_modelo" / "tabelas" / "5_generalizacao_por_grafo.csv").exists():
        tab_ponta()
    else:
        print("generalização por grafo ausente (rode scripts/train_poc.py)")
    if (ROOT / "results" / "dimacs" / "tabela_completa.csv").exists():
        tab_dimacs()
    else:
        print("resultados do DIMACS ausentes (rode scripts/process_dimacs.py)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
