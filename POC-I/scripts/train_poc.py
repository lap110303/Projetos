"""Prova de conceito do modelo preditivo (objetivo 6 da proposta).

Formulação: classificação binária dos candidatos de cada passo (rótulo 3b da camada B),
treinada apenas nos passos informativos; a política aprendida escolhe o candidato de
maior probabilidade. Detalhes em poc/model.py.

Avaliação:
  1. por passo, nos grafos de validação da camada B (divisão por grafo, 80/20,
     estratificada por modelo gerador e n), contra as linhas de base da EDA;
  2. de ponta a ponta: a política aprendida colore os grafos de validação da camada B
     (comparação com as 4 heurísticas e com o oráculo);
  3. generalização: a política colore os grafos da camada A com réplica 3, que não
     aparecem na camada B, incluindo n > 500 (fora da faixa de treino).
O DIMACS NÃO é usado (conjunto de teste final, reservado).

Saídas em results/poc_modelo/: RELATORIO_MODELO.md, figs/, tabelas/, resumo.json e o
modelo escolhido (modelo.joblib).

Uso (a partir da pasta POC-I):
    python scripts/train_poc.py
    python scripts/train_poc.py --workers 8 --heldout-seed 3
"""
from __future__ import annotations

import argparse
import json
import multiprocessing as mp
import os
import pickle
import platform
import signal
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "scripts"))

import joblib                                            # noqa: E402
import matplotlib.pyplot as plt                          # noqa: E402
import numpy as np                                       # noqa: E402
import pandas as pd                                      # noqa: E402
import sklearn                                           # noqa: E402
from sklearn.ensemble import HistGradientBoostingClassifier  # noqa: E402
from sklearn.inspection import permutation_importance   # noqa: E402
from sklearn.linear_model import LogisticRegression     # noqa: E402
from sklearn.metrics import roc_auc_score               # noqa: E402
from sklearn.model_selection import train_test_split    # noqa: E402
from sklearn.pipeline import make_pipeline              # noqa: E402
from sklearn.preprocessing import StandardScaler        # noqa: E402
from threadpoolctl import threadpool_limits              # noqa: E402

import eda                                               # noqa: E402
from eda import HCOLOR, HNAME, INK2, ORACLE_COLOR, Report, model_label, num, pct, pct1  # noqa: E402
from poc.graphs import stable_seed                       # noqa: E402
from poc.heuristics import HEURISTICS, is_valid_coloring  # noqa: E402
from poc.model import (FEATURE_NAMES, GLOBAL_FEATURES, pick_per_step,  # noqa: E402
                       run_learned_policy, training_table)
from poc.storage import load_graph                       # noqa: E402

SPLIT_SEED = 2026
MODEL_COLOR = "#4a3aa7"          # política aprendida (violeta); oráculo usa a mesma família
MODELS = {
    "Regressão logística": lambda: make_pipeline(
        StandardScaler(), LogisticRegression(max_iter=3000, C=1.0)),
    "Gradient boosting": lambda: HistGradientBoostingClassifier(
        max_iter=300, learning_rate=0.1, max_leaf_nodes=31, early_stopping=False,
        random_state=0),
}


# --------------------------------------------------------------------------- #
# Avaliação da política em paralelo
# --------------------------------------------------------------------------- #
_MODEL = None


def _init_worker(model_bytes: bytes) -> None:
    global _MODEL
    signal.signal(signal.SIGINT, signal.SIG_IGN)
    threadpool_limits(1)                      # predições de 2–4 linhas: sem threads
    _MODEL = pickle.loads(model_bytes)


def _eval_graph(args: tuple[str, str]) -> dict:
    gid, path = args
    A = load_graph(path)
    r = run_learned_policy(_MODEL, A, stable_seed(gid, "random-order"))
    if not is_valid_coloring(A, r.colors):
        raise RuntimeError(f"coloração inválida da política aprendida: {gid}")
    return {"graph_id": gid, "colors_model": r.n_colors, "seconds_model": r.seconds,
            "model_calls": r.model_calls,
            **{f"follow_{h}": r.follows[h] / A.shape[0] for h in HEURISTICS}}


def evaluate_policy(model, graph_ids: list[str], data: Path, workers: int,
                    label: str) -> pd.DataFrame:
    # maiores primeiro: equilibra a carga entre os processos
    sizes = {g: int(g.split("_n")[1].split("_")[0]) for g in graph_ids}
    tasks = [(g, str(data / "graphs" / f"{g}.npz"))
             for g in sorted(graph_ids, key=sizes.get, reverse=True)]
    t0 = time.perf_counter()
    rows = []
    with mp.Pool(workers, initializer=_init_worker, initargs=(pickle.dumps(model),)) as pool:
        for i, row in enumerate(pool.imap_unordered(_eval_graph, tasks), 1):
            rows.append(row)
            if i % max(1, len(tasks) // 10) == 0 or i == len(tasks):
                print(f"  [{label}] {i}/{len(tasks)} grafos "
                      f"({time.perf_counter() - t0:.0f} s)", flush=True)
    return pd.DataFrame(rows)


# --------------------------------------------------------------------------- #
# Métricas por passo
# --------------------------------------------------------------------------- #
def step_accuracy(pick: np.ndarray, meta: pd.DataFrame, model_of: pd.Series) -> dict:
    lab = meta["label"].to_numpy()[pick]
    fam = meta["graph_id"].map(model_of).to_numpy()[pick]
    return {"geral": lab.mean(), "ER": lab[fam == "er"].mean(), "BA": lab[fam == "ba"].mean()}


def baseline_picks(meta: pd.DataFrame, X: np.ndarray) -> dict[str, np.ndarray]:
    """Máscaras de escolha das linhas de base (uma linha por passo)."""
    sid = meta["step_id"].to_numpy()
    out = {}
    for h in ("rlf", "dsatur", "welsh_powell", "random"):
        out[f"seguir {HNAME[h]}"] = X[:, FEATURE_NAMES.index(f"by_{h}")] == 1
    # min(avail_color): candidato de menor avail_color (1º na ordem de prioridade)
    out["min(avail_color), desempate por prioridade"] = pick_per_step(-X[:, FEATURE_NAMES.index("avail_color")], sid)
    return out


# --------------------------------------------------------------------------- #
def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="Prova de conceito do modelo.")
    ap.add_argument("--data", type=Path, default=ROOT / "data")
    ap.add_argument("--out", type=Path, default=ROOT / "results" / "poc_modelo")
    ap.add_argument("--workers", type=int, default=max(1, (os.cpu_count() or 2) // 2))
    ap.add_argument("--heldout-seed", type=int, default=3,
                    help="réplica da camada A usada para testar generalização (não está em B)")
    ap.add_argument("--val-frac", type=float, default=0.2)
    ap.add_argument("--overwrite-model", action="store_true",
                    help="substitui modelo.joblib mesmo que o novo modelo seja diferente "
                         "(o DIMACS foi avaliado com o modelo salvo)")
    args = ap.parse_args(argv)
    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    t_start = time.perf_counter()
    rep = Report(args.out)

    # ------------------------------------------------------------ dados
    A = eda.load_tier(args.data, "A")
    O = pd.read_parquet(args.data / "B" / "oracle")
    R = pd.read_parquet(args.data / "B" / "rollouts")
    model_of = A.set_index("graph_id")["model"]
    strat = O["model"] + "_" + O["n"].astype(str)
    tr_ids, va_ids = train_test_split(O["graph_id"], test_size=args.val_frac,
                                      stratify=strat, random_state=SPLIT_SEED)
    tr_ids, va_ids = set(tr_ids), set(va_ids)
    X, y, meta = training_table(R, A)
    del R
    is_tr = meta["graph_id"].isin(tr_ids).to_numpy()
    is_va = meta["graph_id"].isin(va_ids).to_numpy()
    print(f"treino: {is_tr.sum():,} linhas; validação: {is_va.sum():,} linhas", flush=True)

    rep.md("# Prova de conceito do modelo\n\n"
           "Gerado automaticamente por `scripts/train_poc.py`. Formulação sugerida na seção 7 "
           "da análise exploratória (`results/eda/RELATORIO_EDA.md`). O DIMACS não é usado.\n")
    rep.md("## 1. Formulação e dados\n")
    rep.md(
        "A cada passo da construção gulosa, as 4 heurísticas sugerem candidatos (até 4 "
        "vértices distintos). Um classificador binário estima a probabilidade de cada "
        "candidato ser ótimo (rótulo 3b da camada B) e a **política aprendida** colore o "
        "candidato de maior probabilidade (desempate DSATUR > RLF > WP > aleatória). O treino "
        "usa apenas os **passos informativos**.\n")
    n_steps = lambda m: meta.loc[m, "step_id"].nunique()   # noqa: E731
    split = pd.DataFrame({
        "grafos": [len(tr_ids), len(va_ids)],
        "ER": [sum(model_of[g] == "er" for g in tr_ids), sum(model_of[g] == "er" for g in va_ids)],
        "BA": [sum(model_of[g] == "ba" for g in tr_ids), sum(model_of[g] == "ba" for g in va_ids)],
        "passos informativos": [n_steps(is_tr), n_steps(is_va)],
        "candidatos (linhas)": [int(is_tr.sum()), int(is_va.sum())],
        "rótulo 1": [y[is_tr].mean(), y[is_va].mean()],
    }, index=pd.Index(["treino", "validação"], name="conjunto"))
    rep.md(f"Divisão **por grafo** ({pct(1 - args.val_frac)}/{pct(args.val_frac)}), "
           f"estratificada por modelo gerador e n (semente {SPLIT_SEED}):\n")
    rep.table(split, "1_divisao", {"rótulo 1": pct1})
    rep.md(f"Atributos ({len(FEATURE_NAMES)}): do candidato (`degree`, `saturation`, "
           "`uncolored_nb`, `colored_nb`, `avail_color`, `opens_new_color`); do passo "
           "(`n_colors_before`, `step_frac`, `n_candidates`); relativos (`avail_rel` = "
           "avail_color / cores já usadas, `sat_rel`, `unc_frac` = uncolored_nb / grau, "
           "`deg_rel` = grau / n); posição do candidato entre os do passo (`*_rank`, posto "
           "percentual, e `*_dmin`, diferença para o mínimo do passo); quais heurísticas o "
           "sugeriram (`by_*`); e um atributo global por bloco redundante da EDA "
           "(`g_n`, `g_density`, `g_deg_cv`, `g_clust_ratio`).\n")

    # ------------------------------------------------------------ treino
    fitted, rows, sid_va = {}, [], meta.loc[is_va, "step_id"].to_numpy()
    meta_va, meta_tr = meta[is_va].reset_index(drop=True), meta[is_tr].reset_index(drop=True)
    for name, make in MODELS.items():
        t0 = time.perf_counter()
        clf = make().fit(X[is_tr], y[is_tr])
        fit_s = time.perf_counter() - t0
        fitted[name] = clf
        p_va = clf.predict_proba(X[is_va])[:, 1]
        p_tr = clf.predict_proba(X[is_tr])[:, 1]
        acc_va = step_accuracy(pick_per_step(p_va, sid_va), meta_va, model_of)
        acc_tr = step_accuracy(pick_per_step(p_tr, meta_tr["step_id"].to_numpy()), meta_tr,
                               model_of)
        rows.append({"método": name, **{f"acerto {k}": v for k, v in acc_va.items()},
                     "acerto no treino": acc_tr["geral"],
                     "AUC (candidatos)": roc_auc_score(y[is_va], p_va),
                     "tempo de treino (s)": fit_s})
        print(f"{name}: acerto val {acc_va['geral']:.3f} (treino {acc_tr['geral']:.3f}), "
              f"{fit_s:.1f} s", flush=True)
    for name, mask in baseline_picks(meta_va, X[is_va]).items():
        acc = step_accuracy(mask, meta_va, model_of)
        rows.append({"método": name, **{f"acerto {k}": v for k, v in acc.items()}})
    rand = meta_va.groupby("step_id")["label"].mean()
    fam = meta_va.drop_duplicates("step_id").set_index("step_id")["graph_id"].map(model_of)
    rows.append({"método": "candidato ao acaso", "acerto geral": rand.mean(),
                 "acerto ER": rand[fam == "er"].mean(), "acerto BA": rand[fam == "ba"].mean()})
    steps_tab = pd.DataFrame(rows).set_index("método").sort_values("acerto geral",
                                                                   ascending=False)
    rep.md("## 2. Desempenho por passo (validação)\n")
    rep.md("Fração dos passos informativos de validação em que o candidato escolhido é "
           "ótimo. As linhas de base são as da seção 6 da EDA, recalculadas nos mesmos "
           "passos.\n")
    rep.table(steps_tab, "2_acerto_por_passo",
              {**{c: pct1 for c in steps_tab.columns if c.startswith("acerto")},
               "AUC (candidatos)": ".3f", "tempo de treino (s)": ".1f"})
    best_name = steps_tab.loc[list(MODELS)].sort_values("acerto geral").index[-1]
    rep.facts["step_accuracy"] = steps_tab.to_dict(orient="index")
    rep.facts["best_model"] = best_name

    fig, ax = plt.subplots(figsize=(9, 0.42 * len(steps_tab) + 1))
    order = steps_tab.index[::-1]
    colors = [MODEL_COLOR if m in MODELS else
              HCOLOR.get({v: k for k, v in HNAME.items()}.get(m.replace("seguir ", ""), ""),
                         "#c3c2b7") for m in order]
    ax.barh(range(len(order)), steps_tab.loc[order, "acerto geral"], color=colors, height=0.6)
    for i, v in enumerate(steps_tab.loc[order, "acerto geral"]):
        ax.text(v + 0.005, i, pct1(v), va="center", fontsize=8, color=INK2)
    ax.set_yticks(range(len(order)), order)
    ax.xaxis.set_major_formatter(plt.matplotlib.ticker.PercentFormatter(1.0, 0))
    ax.set_xlim(0, 1.05)
    ax.set_xlabel("acerto nos passos informativos de validação")
    ax.grid(axis="y", visible=False)
    ax.set_title("Acerto por passo: modelos (violeta) e linhas de base")
    rep.fig(fig, "2_acerto_por_passo", "Figura 1 — Acerto por passo na validação.")

    # ------------------------------------------------------------ importância
    rep.md("## 3. Importância dos atributos\n")
    rng = np.random.default_rng(0)
    sub = rng.choice(np.flatnonzero(is_va), size=min(60_000, int(is_va.sum())), replace=False)
    imp = permutation_importance(fitted[best_name], X[sub], y[sub], scoring="roc_auc",
                                 n_repeats=3, random_state=0, n_jobs=1)
    imp_tab = (pd.DataFrame({"queda de AUC": imp.importances_mean,
                             "desvio": imp.importances_std}, index=pd.Index(FEATURE_NAMES,
                                                                             name="atributo"))
               .sort_values("queda de AUC", ascending=False))
    rep.md(f"Importância por permutação ({best_name}, {num(len(sub), ',')} candidatos de "
           f"validação, 3 repetições): quanto a AUC cai quando o atributo é embaralhado. "
           f"Atributos correlacionados dividem a importância entre si.\n")
    rep.table(imp_tab.head(15), "3_importancia", {"queda de AUC": ".4f", "desvio": ".4f"})
    rep.save_csv(imp_tab, "3_importancia_completa")
    rep.facts["top_features"] = imp_tab.index[:5].tolist()
    top = imp_tab.head(15)[::-1]
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.barh(range(len(top)), top["queda de AUC"], xerr=top["desvio"], color=MODEL_COLOR,
            height=0.6, error_kw={"ecolor": INK2, "linewidth": 1})
    ax.set_yticks(range(len(top)), top.index)
    ax.set_xlabel("queda da AUC ao embaralhar o atributo")
    ax.grid(axis="y", visible=False)
    ax.set_title(f"15 atributos mais importantes — {best_name}")
    rep.fig(fig, "3_importancia", "Figura 2 — Importância por permutação.")

    # ------------------------------------------------------------ política em B (validação)
    rep.md("## 4. Política aprendida nos grafos de validação da camada B\n")
    rep.md("Cada modelo é usado como regra de escolha para colorir, do início ao fim, os "
           "grafos de validação. Toda coloração é validada. Diferenças em nº de cores: "
           "negativo = a política usou **menos** cores.\n")
    OB = O[O.graph_id.isin(va_ids)].set_index("graph_id")
    OB["best_h"] = OB[[f"colors_{h}" for h in HEURISTICS]].min(axis=1)
    pol_rows, pol_B = [], {}
    for name, clf in fitted.items():
        res = evaluate_policy(clf, sorted(va_ids), args.data, args.workers, name)
        d = OB.join(res.set_index("graph_id"), how="inner")
        pol_B[name] = d
        rep.save_csv(d, f"4_politica_por_grafo_{'gb' if 'boosting' in name else 'lr'}")
        for fam_lab, dd in (("geral", d), ("ER", d[d.model == "er"]), ("BA", d[d.model == "ba"])):
            dh = dd["colors_model"] - dd["best_h"]
            pol_rows.append({
                "método": name, "grafos": fam_lab, "n": len(dd),
                "< melhor heurística": (dh < 0).mean(), "= melhor": (dh == 0).mean(),
                "> melhor": (dh > 0).mean(),
                "Δ médio vs melhor": dh.mean(),
                "Δ médio vs RLF": (dd["colors_model"] - dd["colors_rlf"]).mean(),
                "Δ médio vs DSATUR": (dd["colors_model"] - dd["colors_dsatur"]).mean(),
                "Δ médio vs oráculo": (dd["colors_model"] - dd["oracle_colors"]).mean(),
            })
    pol_tab = pd.DataFrame(pol_rows).set_index(["método", "grafos"])
    rep.table(pol_tab, "4_politica_validacao_B",
              {"< melhor heurística": pct, "= melhor": pct, "> melhor": pct,
               **{c: "+.2f" for c in pol_tab.columns if c.startswith("Δ")}})
    rep.facts["policy_B"] = {f"{m}|{g}": v for (m, g), v in
                             pol_tab.to_dict(orient="index").items()}

    d = pol_B[best_name]
    fig, axes = plt.subplots(1, 2, figsize=(12, 3.8), sharey=True)
    for ax, fam_code in zip(axes, ("er", "ba")):
        dd = d[d.model == fam_code]
        series = {"melhor heurística": dd["colors_model"] - dd["best_h"],
                  "oráculo": dd["colors_model"] - dd["oracle_colors"]}
        lo = int(min(s.min() for s in series.values()))
        hi = int(max(s.max() for s in series.values()))
        bins = np.arange(lo - 0.5, hi + 1.5)
        for (lab, s), col, off in zip(series.items(), (INK2, ORACLE_COLOR), (-0.2, 0.2)):
            cnt = np.histogram(s, bins=bins)[0]
            ax.bar(bins[:-1] + 0.5 + off, cnt, width=0.4, color=col, label=f"vs {lab}")
        ax.axvline(0, color="#c3c2b7", linewidth=1)
        ax.set_title(f"{model_label(fam_code)} ({len(dd)} grafos)")
        ax.set_xlabel("cores da política − cores da referência")
        eda.legend_below(ax, ncol=2)
    axes[0].set_ylabel("nº de grafos")
    fig.tight_layout()
    rep.fig(fig, "4_politica_validacao_B", f"Figura 3 — Política aprendida ({best_name}) "
            "nos grafos de validação da camada B: diferença de cores para a melhor das 4 "
            "heurísticas e para o oráculo.")
    fol = d[[f"follow_{h}" for h in HEURISTICS]].groupby(d["model"]).mean()
    fol.columns = [HNAME[h] for h in HEURISTICS]
    fol.index = [model_label(m) for m in fol.index]
    fol.index.name = "modelo"
    rep.md(f"Com que frequência a política ({best_name}) seguiu a sugestão de cada "
           "heurística (sugestões coincidentes contam para todas):\n")
    rep.table(fol, "4_politica_segue", {c: pct for c in fol.columns})

    # ------------------------------------------------------------ generalização (A, réplica)
    rep.md(f"## 5. Generalização: grafos da camada A com réplica {args.heldout_seed}\n")
    H_ids = A.loc[(A.seed == args.heldout_seed), "graph_id"].tolist()
    AH = A.set_index("graph_id").loc[H_ids].copy()
    res = evaluate_policy(fitted[best_name], H_ids, args.data, args.workers, "réplica")
    AH = AH.join(res.set_index("graph_id"))
    AH["dh"] = AH["colors_model"] - AH["best"]
    AH["dh_pct"] = AH["dh"] / AH["best"]
    rep.save_csv(AH[["model", "n", "param", "seed", "best", *[f"colors_{h}" for h in HEURISTICS],
                     "colors_model", "seconds_model", "seconds_rlf", "dh", "dh_pct"]],
                 "5_generalizacao_por_grafo")
    AH["faixa"] = np.where(AH.n <= 500, "n ≤ 500 (faixa do treino)", "n > 500 (fora da faixa)")
    g = AH.groupby([AH["model"].map(model_label), "faixa"])
    gen = pd.DataFrame({
        "grafos": g.size(),
        "< melhor heurística": g["dh"].apply(lambda s: (s < 0).mean()),
        "= melhor": g["dh"].apply(lambda s: (s == 0).mean()),
        "> melhor": g["dh"].apply(lambda s: (s > 0).mean()),
        "Δ médio vs melhor": g["dh"].mean(),
        "Δ% médio vs melhor": g["dh_pct"].mean(),
        "Δ médio vs RLF": g.apply(lambda x: (x["colors_model"] - x["colors_rlf"]).mean()),
        "Δ médio vs DSATUR": g.apply(lambda x: (x["colors_model"] - x["colors_dsatur"]).mean()),
        "tempo médio política (s)": g["seconds_model"].mean(),
        "tempo médio RLF (s)": g["seconds_rlf"].mean(),
    })
    gen.index.names = ["modelo", "faixa"]
    rep.md(f"Estes {len(AH)} grafos **não estão na camada B** (réplicas 0–2) e nunca foram "
           f"vistos no treino; os com n > 500 estão fora da faixa de tamanhos do treino. "
           f"Política: {best_name}.\n")
    rep.table(gen, "5_generalizacao",
              {"< melhor heurística": pct, "= melhor": pct, "> melhor": pct,
               "Δ% médio vs melhor": lambda v: ("+" if v >= 0.0005 else "") +
               pct1(v).replace("-0,0%", "0,0%"),
               "tempo médio política (s)": ".2f", "tempo médio RLF (s)": ".3f",
               **{c: "+.2f" for c in gen.columns if c.startswith("Δ médio")}})
    rep.facts["generalization"] = {f"{a}|{b}": v for (a, b), v in
                                   gen.to_dict(orient="index").items()}

    fig, axes = plt.subplots(1, 2, figsize=(12, 3.8), sharey=True)
    for ax, fam_code in zip(axes, ("er", "ba")):
        dd = AH[AH.model == fam_code]
        for lab, col, ref in (("vs melhor heurística", MODEL_COLOR, "best"),
                              ("vs RLF", HCOLOR["rlf"], "colors_rlf"),
                              ("vs DSATUR", HCOLOR["dsatur"], "colors_dsatur")):
            s = ((dd["colors_model"] - dd[ref]) / dd[ref]).groupby(dd["n"]).mean()
            ax.plot(s.index, s, color=col, label=lab, marker="o", markersize=3)
        ax.axhline(0, color="#c3c2b7", linewidth=1)
        ax.axvspan(500, 1000, color="#f0efec", zorder=0)
        ax.set_title(f"{model_label(fam_code)} — réplica {args.heldout_seed}")
        ax.set_xlabel("n (faixa cinza: fora da faixa de treino)")
        ax.yaxis.set_major_formatter(plt.matplotlib.ticker.PercentFormatter(1.0, 0))
        eda.legend_below(ax, ncol=3)
    axes[0].set_ylabel("(cores da política − referência) / referência")
    fig.tight_layout()
    rep.fig(fig, "5_generalizacao", "Figura 4 — Diferença relativa de cores da política "
            "aprendida por n, nos grafos nunca vistos (negativo = melhor que a referência).")

    # ------------------------------------------------------------ conclusões
    sb = steps_tab
    base_name = sb.drop(index=list(MODELS)).index[0]
    b_best = pol_tab.loc[(best_name, "geral")]
    b_er, b_ba = pol_tab.loc[(best_name, "ER")], pol_tab.loc[(best_name, "BA")]
    out_rng = AH[AH.n > 500]
    inf_per_graph = meta_va.groupby("graph_id")["step_id"].nunique()
    err = 1 - sb.at[best_name, "acerto geral"]
    fol_er = fol.loc[model_label("er")] if model_label("er") in fol.index else None
    fol_ba = fol.loc[model_label("ba")] if model_label("ba") in fol.index else None
    rep.md("## 6. Conclusões da prova de conceito\n")
    items = [
        f"**Por passo.** O {best_name} acerta {pct1(sb.at[best_name, 'acerto geral'])} dos "
        f"passos informativos de validação, contra {pct1(sb.at[base_name, 'acerto geral'])} "
        f"da melhor linha de base (`{base_name}`), "
        f"{pct1(sb.at['seguir RLF', 'acerto geral'])} do candidato do RLF e "
        f"{pct1(sb.at['candidato ao acaso', 'acerto geral'])} ao acaso "
        f"(treino: {pct1(sb.at[best_name, 'acerto no treino'])}). Os atributos mais "
        f"importantes são {', '.join(f'`{x}`' for x in rep.facts['top_features'][:3])}.",
        f"**De ponta a ponta (validação B).** A política usa menos cores que a melhor das 4 "
        f"heurísticas em {pct(b_best['< melhor heurística'])} dos grafos, o mesmo número em "
        f"{pct(b_best['= melhor'])} e mais em {pct(b_best['> melhor'])} "
        f"(ER: {pct(b_er['< melhor heurística'])} / {pct(b_er['= melhor'])} / "
        f"{pct(b_er['> melhor'])}; BA: {pct(b_ba['< melhor heurística'])} / "
        f"{pct(b_ba['= melhor'])} / {pct(b_ba['> melhor'])}). Em média fica "
        f"{num(b_best['Δ médio vs oráculo'], '+.2f')} cores acima do oráculo.",
        f"**Leitura: o modelo aprendeu a escolher a heurística certa para a família, não "
        f"ainda a superá-la.** " + (
            f"Ao colorir, ele seguiu o candidato do RLF em {pct(fol_er['RLF'])} dos passos "
            f"no ER e o do DSATUR em {pct(fol_ba['DSATUR'])} no BA — exatamente as "
            f"vencedoras de cada família na EDA. " if fol_er is not None and fol_ba is not None
            else "") +
        f"O alto acerto por passo não se converte em menos cores porque os erros se "
        f"acumulam: os grafos de validação têm em média {num(inf_per_graph.mean(), '.0f')} "
        f"passos informativos, e {pct1(err)} de erro significa ~{num(err * inf_per_graph.mean(), '.1f')} "
        f"escolhas não ótimas por grafo — cada uma pode custar uma cor, e o ganho do oráculo "
        f"sobre a melhor heurística nesses grafos é, em média, de "
        f"{num((OB['best_h'] - OB['oracle_colors']).mean())} cores. Superar as "
        f"heurísticas exige acerto por passo muito mais próximo de 100% nos passos que "
        f"decidem o resultado, ou uma métrica de treino ligada ao nº final de cores.",
        f"**Generalização.** Nos {len(AH)} grafos nunca vistos, a política supera a melhor "
        f"heurística em {pct((AH.dh < 0).mean())} e perde em {pct((AH.dh > 0).mean())}; "
        f"com n > 500 (fora da faixa do treino), supera em {pct((out_rng.dh < 0).mean())} e "
        f"perde em {pct((out_rng.dh > 0).mean())}.",
        "**Custo.** A política é mais lenta que as heurísticas: a cada passo calcula as 4 "
        "sugestões e chama o modelo (ver tabela da seção 5). Para a prova de conceito isso "
        "é aceitável; no POC II, vale calcular os candidatos de forma incremental.",
        "**Limitações e próximos passos (POC II).** (i) O treino usa estados da trajetória "
        "do oráculo; ao seguir a própria política, o modelo visita outros estados (mudança "
        "de distribuição) — uma extensão natural é rotular também os estados visitados pela "
        "política (DAgger). (ii) Todos os erros pesam igual no treino, mas só alguns mudam o "
        "nº final de cores; ponderar os exemplos pela diferença de valor entre candidatos "
        "(coluna `rollout_colors`) ou aprender a ordenar candidatos pode focar o modelo nos "
        "passos decisivos. (iii) Os candidatos se limitam às sugestões das 4 heurísticas: o "
        "modelo não pode escolher um vértice que nenhuma delas sugere. (iv) Só grafos ER e "
        "BA no treino; o DIMACS dirá se a política generaliza para outras estruturas.",
    ]
    rep.md("\n".join(f"{i}. {t}" for i, t in enumerate(items, 1)) + "\n")

    # ------------------------------------------------------------ gravação
    (args.out / "RELATORIO_MODELO.md").write_text("\n".join(rep.parts), encoding="utf-8")
    model_path = args.out / "modelo.joblib"
    if model_path.exists() and not args.overwrite_model:
        # o modelo salvo é o que foi usado no teste final (DIMACS): um retreino só pode
        # mantê-lo se for o mesmo modelo (mesmas probabilidades na validação)
        stored = joblib.load(model_path)
        same = (stored["name"] == best_name and np.array_equal(
            stored["model"].predict_proba(X[is_va]), fitted[best_name].predict_proba(X[is_va])))
        if not same:
            raise SystemExit("o modelo treinado difere de results/poc_modelo/modelo.joblib, "
                             "usado no teste final; use --overwrite-model para substituí-lo")
        print("modelo idêntico ao salvo em modelo.joblib (mantido)")
    else:
        joblib.dump({"model": fitted[best_name], "name": best_name,
                     "feature_names": FEATURE_NAMES}, model_path)
    rep.facts["environment"] = {"python": platform.python_version(),
                                "scikit-learn": sklearn.__version__,
                                "numpy": np.__version__, "pandas": pd.__version__}
    rep.facts["args"] = {k: str(v) for k, v in vars(args).items()}
    rep.facts["minutes"] = (time.perf_counter() - t_start) / 60
    (args.out / "resumo.json").write_text(
        json.dumps(rep.facts, indent=2, ensure_ascii=False, default=float), encoding="utf-8")
    print(f"relatório em {args.out / 'RELATORIO_MODELO.md'} "
          f"({(time.perf_counter() - t_start) / 60:.1f} min)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
