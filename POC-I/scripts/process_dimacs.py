"""Teste final no DIMACS: heurísticas, oráculo e política aprendida (modelo congelado).

Para cada instância do catálogo (scripts/download_dimacs.py):
  * atributos globais do grafo;
  * as 4 heurísticas (toda coloração validada);
  * o oráculo por rollout (opção 3b) nas instâncias com n ≤ --oracle-max-n (padrão 500,
    o mesmo limite da camada B) — referência do quanto é possível melhorar;
  * a política aprendida com o modelo de results/poc_modelo/modelo.joblib, SEM nenhum
    ajuste (o DIMACS é o conjunto de teste final e é usado uma única vez).

Uso único: o resultado de cada instância é gravado em data/dimacs/resultados/<nome>.json
(com o sha256 do modelo usado) e instâncias já processadas são puladas em execuções
seguintes; assim a política nunca é aplicada duas vezes à mesma instância. Ao final, o
relatório results/dimacs/RELATORIO_DIMACS.md é gerado a partir desses arquivos.

Uso (a partir da pasta POC-I):
    python scripts/process_dimacs.py               # processa o que falta e gera o relatório
    python scripts/process_dimacs.py --report-only
"""
from __future__ import annotations

import os

for _var in ("OMP_NUM_THREADS", "OPENBLAS_NUM_THREADS", "MKL_NUM_THREADS"):
    os.environ.setdefault(_var, "1")

import argparse               # noqa: E402
import hashlib                # noqa: E402
import json                   # noqa: E402
import multiprocessing as mp  # noqa: E402
import platform               # noqa: E402
import signal                 # noqa: E402
import sys                    # noqa: E402
import time                   # noqa: E402
import traceback              # noqa: E402
from datetime import datetime  # noqa: E402
from pathlib import Path      # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
sys.path[:0] = [str(ROOT), str(ROOT / "scripts")]

import joblib                 # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402
import numpy as np            # noqa: E402
import pandas as pd           # noqa: E402

import eda                    # noqa: E402
from eda import HCOLOR, HNAME, INK2, Report, num, pct, pct1  # noqa: E402
from poc.features import graph_features  # noqa: E402
from poc.graphs import check_adjacency, stable_seed  # noqa: E402
from poc.heuristics import HEURISTICS, is_valid_coloring, run_heuristic  # noqa: E402
from poc.io_dimacs import read_dimacs  # noqa: E402
from poc.model import global_features_from, run_learned_policy  # noqa: E402
from poc.rollout import rollout_trajectory  # noqa: E402

DIM = ROOT / "data" / "dimacs"
MODEL_PATH = ROOT / "results" / "poc_modelo" / "modelo.joblib"
POLICY_COLOR = "#4a3aa7"
_MODEL = None


def _sha256(path: Path) -> str:
    return hashlib.sha256(path.read_bytes()).hexdigest()


def _init_worker(model_path: str) -> None:
    global _MODEL
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    from threadpoolctl import threadpool_limits
    threadpool_limits(1)
    _MODEL = joblib.load(model_path)["model"]


def process_instance(args: tuple[dict, int, str, str]) -> tuple[str, str | None]:
    """Processa uma instância e grava o JSON. Retorna (nome, erro)."""
    row, oracle_max_n, model_sha, dim = args
    dim = Path(dim)
    name = row["instancia"]
    try:
        A = read_dimacs(dim / "instancias" / row["arquivo"])
        check_adjacency(A)
        n = A.shape[0]
        seed = stable_seed(name, "random-order")
        t0 = time.perf_counter()
        gf = graph_features(A)
        out = {"instancia": name, **{k: (float(v) if isinstance(v, (float, np.floating))
                                         else int(v) if isinstance(v, (int, np.integer,
                                                                       bool, np.bool_))
                                         else v) for k, v in gf.items()},
               "t_features": time.perf_counter() - t0}
        for h in HEURISTICS:
            r = run_heuristic(h, A, seed=seed)
            if not is_valid_coloring(A, r.colors):
                raise RuntimeError(f"coloração inválida ({h})")
            out[f"colors_{h}"], out[f"seconds_{h}"] = int(r.n_colors), r.seconds
        if n <= oracle_max_n:
            r = rollout_trajectory(A, seed=seed)
            if not is_valid_coloring(A, r.colors):
                raise RuntimeError("coloração do oráculo inválida")
            out.update(oracle_colors=int(r.n_colors), oracle_seconds=r.seconds,
                       oracle_rollouts=int(r.n_rollouts))
        r = run_learned_policy(_MODEL, A, seed, global_features_from(gf))
        if not is_valid_coloring(A, r.colors):
            raise RuntimeError("coloração da política aprendida inválida")
        out.update(colors_policy=int(r.n_colors), seconds_policy=r.seconds,
                   model_calls=int(r.model_calls),
                   **{f"follow_{h}": r.follows[h] / n for h in HEURISTICS},
                   model_sha256=model_sha, executado_em=datetime.now().isoformat(timespec="seconds"))
        path = dim / "resultados" / f"{name}.json"
        tmp = path.with_name(path.name + ".tmp")
        tmp.write_text(json.dumps(out, ensure_ascii=False, indent=1), encoding="utf-8")
        tmp.replace(path)
        return name, None
    except Exception:
        return name, traceback.format_exc()


def run_all(dim: Path, workers: int, oracle_max_n: int) -> list[str]:
    cat = pd.read_csv(dim / "catalogo.csv")
    (dim / "resultados").mkdir(parents=True, exist_ok=True)
    done = {p.stem for p in (dim / "resultados").glob("*.json")}
    todo = cat[~cat.instancia.isin(done)].sort_values("bytes", ascending=False)
    model_sha = _sha256(MODEL_PATH)
    print(f"{len(cat)} instâncias, {len(done)} já processadas, {len(todo)} a processar; "
          f"modelo sha256 {model_sha[:12]}…", flush=True)
    failed = []
    if len(todo):
        tasks = [(r, oracle_max_n, model_sha, str(dim)) for r in todo.to_dict(orient="records")]
        t0 = time.perf_counter()
        with mp.Pool(workers, initializer=_init_worker, initargs=(str(MODEL_PATH),)) as pool:
            for i, (name, err) in enumerate(pool.imap_unordered(process_instance, tasks), 1):
                if err:
                    failed.append(name)
                    print(f"  FALHA {name}:\n{err}", flush=True)
                else:
                    print(f"  {i:3d}/{len(tasks)} {name} "
                          f"({(time.perf_counter() - t0) / 60:.1f} min)", flush=True)
    return failed


# --------------------------------------------------------------------------- #
# Relatório
# --------------------------------------------------------------------------- #
def load_results(dim: Path) -> pd.DataFrame:
    cat = pd.read_csv(dim / "catalogo.csv")
    rows = [json.loads(p.read_text(encoding="utf-8"))
            for p in sorted((dim / "resultados").glob("*.json"))]
    d = cat.merge(pd.DataFrame(rows), on="instancia", how="inner", validate="one_to_one")
    d["best_h"] = d[[f"colors_{h}" for h in HEURISTICS]].min(axis=1)
    d["d_best"] = d["colors_policy"] - d["best_h"]
    d["d_best_pct"] = d["d_best"] / d["best_h"]
    d["deg_cv"] = d["deg_std"] / d["deg_mean"]
    d["winner"] = eda.winner_category(d)
    return d


def report(dim: Path, out: Path) -> None:
    d = load_results(dim)
    rep = Report(out)
    sha = d["model_sha256"].unique()
    H = list(HEURISTICS)
    rep.md("# Teste final no DIMACS\n\n"
           "Gerado por `scripts/process_dimacs.py`. A política aprendida da prova de conceito "
           "(`results/poc_modelo/modelo.joblib`, sha256 " + ", ".join(f"`{s[:12]}…`" for s in sha) +
           ") foi aplicada **sem nenhum ajuste**, uma única vez por instância "
           f"(execução em {d.executado_em.min()[:10]}).\n")

    # 1. conjunto
    fam = (d.groupby("familia").agg(instancias=("instancia", "size"), n_min=("n", "min"),
                                    n_max=("n", "max"),
                                    densidade=("density", "median"),
                                    chi_conhecido=("chi_conhecido", lambda s: s.notna().sum()))
           .sort_values("instancias", ascending=False))
    fam.index.name = "família"
    rep.md(f"## 1. Conjunto\n\n{len(d)} instâncias ({fam.chi_conhecido.sum()} com número "
           f"cromático conhecido na fonte), n de {d.n.min()} a {num(d.n.max(), ',')}.\n")
    rep.table(fam, "1_familias", {"densidade": ".3f"})

    # 2. heurísticas
    rep.md("## 2. Heurísticas\n")
    rows = []
    for h in H:
        best = d[f"colors_{h}"] == d.best_h
        alone = best & ((d[[f"colors_{k}" for k in H]].to_numpy() ==
                         d.best_h.to_numpy()[:, None]).sum(1) == 1)
        rows.append({"heurística": HNAME[h], "melhor c/ empate": best.mean(),
                     "melhor sozinha": alone.mean(),
                     "% acima da melhor (média)": (d[f"colors_{h}"] / d.best_h - 1).mean()})
    tab = pd.DataFrame(rows).set_index("heurística")
    rep.table(tab, "2_heuristicas", {"melhor c/ empate": pct, "melhor sozinha": pct,
                                     "% acima da melhor (média)": pct1})
    wf = pd.crosstab(d.familia, d.winner)
    wf.index.name = "família"
    rep.md("\nCategoria vencedora por família (nº de instâncias):\n")
    rep.table(wf, "2_vencedora_por_familia")

    # 3. política aprendida
    rep.md("## 3. Política aprendida × heurísticas\n")
    g = d.groupby("familia")
    pol = pd.DataFrame({
        "instâncias": g.size(),
        "< melhor": g.d_best.apply(lambda s: (s < 0).sum()),
        "= melhor": g.d_best.apply(lambda s: (s == 0).sum()),
        "> melhor": g.d_best.apply(lambda s: (s > 0).sum()),
        "Δ médio": g.d_best.mean(),
        "segue RLF": g.follow_rlf.mean(), "segue DSATUR": g.follow_dsatur.mean(),
    })
    pol.loc["**total**"] = [len(d), (d.d_best < 0).sum(), (d.d_best == 0).sum(),
                            (d.d_best > 0).sum(), d.d_best.mean(), d.follow_rlf.mean(),
                            d.follow_dsatur.mean()]
    pol.index.name = "família"
    rep.md("Comparação com a melhor das 4 heurísticas em cada instância (Δ = cores da "
           "política − cores da melhor heurística; negativo = política melhor). "
           "\"Segue\" = fração dos passos em que a política escolheu a sugestão da heurística.\n")
    rep.table(pol, "3_politica_por_familia",
              {"Δ médio": "+.2f", "segue RLF": pct, "segue DSATUR": pct,
               **{c: ".0f" for c in ("instâncias", "< melhor", "= melhor", "> melhor")}})
    vs = pd.DataFrame({ref: {"política melhor": (d.colors_policy < d[c]).mean(),
                             "empate": (d.colors_policy == d[c]).mean(),
                             "política pior": (d.colors_policy > d[c]).mean(),
                             "Δ médio (cores)": (d.colors_policy - d[c]).mean()}
                       for ref, c in (("melhor heurística", "best_h"), ("RLF", "colors_rlf"),
                                      ("DSATUR", "colors_dsatur"))}).T
    vs.index.name = "referência"
    rep.table(vs, "3_politica_resumo", {"política melhor": pct, "empate": pct,
                                        "política pior": pct, "Δ médio (cores)": "+.2f"})

    # 4. ótimo conhecido
    k = d[d.chi_conhecido.notna()]
    rep.md(f"## 4. Distância ao número cromático conhecido ({len(k)} instâncias)\n")
    opt = pd.DataFrame({lab: {"atinge χ": (k[c] == k.chi_conhecido).mean(),
                              "excesso médio (%)": (k[c] / k.chi_conhecido - 1).mean()}
                        for lab, c in [(HNAME[h], f"colors_{h}") for h in H] +
                        [("política aprendida", "colors_policy")]}).T
    opt.index.name = "método"
    rep.table(opt, "4_otimo_conhecido", {"atinge χ": pct, "excesso médio (%)": pct1})
    if (k.colors_policy < k.chi_conhecido).any() or (k.best_h < k.chi_conhecido).any():
        rep.md("**Atenção:** alguma coloração usou menos cores que o χ da fonte — verificar.\n")

    # 5. oráculo
    o = d[d.oracle_colors.notna()] if "oracle_colors" in d else d.iloc[:0]
    if len(o):
        go = o.best_h - o.oracle_colors
        rep.md(f"## 5. Oráculo por rollout (n ≤ 500, {len(o)} instâncias)\n")
        rep.md(f"O oráculo usa menos cores que a melhor heurística em {pct((go > 0).mean())} "
               f"dessas instâncias (ganho médio {num(go.mean())} cores, máximo "
               f"{int(go.max())}); a política aprendida fica, em média, "
               f"{num((o.colors_policy - o.oracle_colors).mean())} cores acima do oráculo. "
               f"Nunca pior que min(DSATUR, RLF): "
               f"{'sim' if (o.oracle_colors <= o[['colors_dsatur', 'colors_rlf']].min(axis=1)).all() else 'NÃO'}.\n")

    # 6. tempo
    d["faixa_n"] = pd.cut(d.n, [0, 200, 500, 1000, 2500, 11000],
                          labels=["≤ 200", "201–500", "501–1.000", "1.001–2.500", "> 2.500"])
    tt = d.groupby("faixa_n", observed=True).agg(
        instancias=("instancia", "size"), **{f"{HNAME[h]} (s)": (f"seconds_{h}", "median")
                                             for h in ("dsatur", "rlf")},
        **{"política (s)": ("seconds_policy", "median")})
    tt.index.name = "n"
    rep.md("## 6. Tempo (mediana por faixa de n)\n")
    rep.table(tt, "6_tempo", {c: ".3f" for c in tt.columns if "(s)" in c})

    # 7. figuras
    fig, ax = plt.subplots(figsize=(8, 5))
    if (ROOT / "data" / "A" / "graphs").exists():
        A = eda.load_tier(ROOT / "data", "A")
        for fam_code, col, lab in (("er", "#86b6ef", "treino: Erdős–Rényi (camada A)"),
                                   ("ba", "#104281", "treino: Barabási–Albert (camada A)")):
            s = A[A.model == fam_code]
            ax.scatter(s.density, s.deg_cv, s=8, alpha=0.35, color=col, label=lab,
                       linewidths=0)
    ax.scatter(d.density, d.deg_cv, s=34, color="#eb6834", edgecolors="#fcfcfb",
               linewidths=0.6, label="DIMACS (teste)")
    ax.set_xscale("log")
    ax.set_yscale("log")
    ax.set_xlabel("densidade (escala log)")
    ax.set_ylabel("coef. de variação do grau (escala log)")
    ax.legend(loc="lower left")
    ax.set_title("Instâncias de teste no espaço de atributos do treino")
    rep.md("## 7. Figuras\n")
    rep.fig(fig, "1_espaco_atributos", "Figura 1 — Densidade e heterogeneidade dos graus: "
            "grafos de treino (ER e BA) e instâncias DIMACS.")

    order = pol.drop(index="**total**").sort_values("Δ médio").index
    fig, ax = plt.subplots(figsize=(8, 0.42 * len(order) + 1.2))
    for i, f in enumerate(order):
        s = d[d.familia == f].d_best
        jitter = (np.arange(len(s)) - (len(s) - 1) / 2) * 0.06
        ax.scatter(s, np.full(len(s), i) + jitter, color=POLICY_COLOR, s=22, alpha=0.8)
    ax.axvline(0, color="#c3c2b7", linewidth=1)
    ax.set_yticks(range(len(order)), order)
    ax.set_xlabel("cores da política − cores da melhor heurística (negativo = política melhor)")
    ax.grid(axis="y", visible=False)
    rep.fig(fig, "2_politica_por_familia", "Figura 2 — Diferença de cores entre a política "
            "aprendida e a melhor heurística, por instância e família.")

    cols = ["instancia", "familia", "n", "m", "density", "chi_conhecido", *[f"colors_{h}" for h in H],
            "oracle_colors", "colors_policy", "d_best", "seconds_rlf", "seconds_policy",
            "follow_rlf", "follow_dsatur"]
    d[[c for c in cols if c in d]].sort_values(["familia", "instancia"]).to_csv(
        out / "tabela_completa.csv", index=False, encoding="utf-8")
    facts = {"instancias": len(d), "chi_conhecido": int(len(k)),
             "politica_vs_melhor": {"melhor": int((d.d_best < 0).sum()),
                                    "empate": int((d.d_best == 0).sum()),
                                    "pior": int((d.d_best > 0).sum()),
                                    "delta_medio": float(d.d_best.mean())},
             "vs": vs.to_dict(orient="index"), "otimo": opt.to_dict(orient="index"),
             "heuristicas": tab.to_dict(orient="index"),
             "oraculo": ({"n": int(len(o)), "melhor_que_heuristica": float((go > 0).mean()),
                          "ganho_medio": float(go.mean()),
                          "politica_menos_oraculo": float((o.colors_policy - o.oracle_colors).mean())}
                         if len(o) else None),
             "modelo_sha256": list(sha), "ambiente": {"python": platform.python_version()}}
    rep.facts.update(facts)
    (out / "RELATORIO_DIMACS.md").write_text("\n".join(rep.parts), encoding="utf-8")
    (out / "resumo.json").write_text(json.dumps(rep.facts, indent=2, ensure_ascii=False,
                                                default=float), encoding="utf-8")
    print(f"relatório em {out / 'RELATORIO_DIMACS.md'}")


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Teste final no DIMACS.")
    ap.add_argument("--workers", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    ap.add_argument("--oracle-max-n", type=int, default=500)
    ap.add_argument("--report-only", action="store_true")
    ap.add_argument("--out", type=Path, default=ROOT / "results" / "dimacs")
    ap.add_argument("--dimacs-dir", type=Path, default=DIM,
                    help="pasta com catalogo.csv e instancias/ (testes usam uma cópia)")
    args = ap.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    failed = [] if args.report_only else run_all(args.dimacs_dir, args.workers,
                                                 args.oracle_max_n)
    report(args.dimacs_dir, args.out)
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
