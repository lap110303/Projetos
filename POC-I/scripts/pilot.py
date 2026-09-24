"""Piloto: mede tempo, memória e espaço em disco numa amostra de cada camada
e projeta o custo da base completa.

Amostragem estratificada por (modelo, n): em cada estrato sorteiam-se k grafos.
Estimativa do total de uma grandeza X na camada:
    T = soma_h  N_h * média_amostral_h(X)
com erro-padrão calculado nos estratos com >= 2 sorteios. Além da amostra,
os piores casos de cada camada são medidos explicitamente.

Uso (a partir da pasta POC-I):
    python scripts/pilot.py                 # amostra padrão
    python scripts/pilot.py --per-stratum-a 3 --per-stratum-b 2
"""
from __future__ import annotations

import argparse
import json
import platform
import random
import resource
import sys
import time
from collections import defaultdict
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

import numpy as np          # noqa: E402
import pandas as pd         # noqa: E402

from poc.graphs import GraphSpec                          # noqa: E402
from poc.grids import tier_a, tier_b, tier_s              # noqa: E402
from poc.pipeline import process_graph, process_rollout   # noqa: E402

OUT = ROOT / "results" / "pilot"
TMP = OUT / "tmp_graphs"


# --------------------------------------------------------------------------- #
def stratified_sample(specs, k, rng):
    """Sorteia k grafos por estrato (modelo, n). Retorna (amostra, tamanhos)."""
    strata = defaultdict(list)
    for g in specs:
        strata[(g.model, g.n)].append(g)
    sample = []
    for key in sorted(strata):
        pool = strata[key]
        sample += rng.sample(pool, min(k, len(pool)))
    return sample, {key: len(v) for key, v in strata.items()}


def with_worst(sample, worst):
    """Une a amostra aos piores casos; devolve (lista, ids_da_amostra)."""
    ids = {g.graph_id for g in sample}
    extra = [g for g in worst if g.graph_id not in ids]
    return sample + extra, ids


def stratified_total(df: pd.DataFrame, col: str, sizes: dict) -> tuple[float, float]:
    """Estimativa do total e erro-padrão (estratos com 1 amostra não entram no EP)."""
    total, var = 0.0, 0.0
    for (model, n), N in sizes.items():
        x = df.loc[(df["model"] == model) & (df["n"] == n) & df["in_sample"], col]
        if len(x) == 0:
            continue
        total += N * x.mean()
        if len(x) >= 2:
            var += N ** 2 * x.var(ddof=1) / len(x) * (1 - len(x) / N)
    return total, var ** 0.5


def peak_rss_mb() -> float:
    return resource.getrusage(resource.RUSAGE_SELF).ru_maxrss / 1024


# --------------------------------------------------------------------------- #
def run_graph_tier(name, specs, k, rng, worst):
    sample, sizes = stratified_sample(specs, k, rng)
    sample, sampled_ids = with_worst(sample, worst)
    worst_ids = {g.graph_id for g in worst}
    rows, runs, steps = [], [], []
    print(f"\n[camada {name}] {len(sample)} grafos medidos de {len(specs)}", flush=True)
    for i, spec in enumerate(sorted(sample, key=lambda g: (g.n, g.model)), 1):
        out = process_graph(spec, graph_dir=TMP)
        t = out.timings
        row = {**out.graph_row,
               "tier": name,
               "in_sample": spec.graph_id in sampled_ids,
               "is_worst_case": spec.graph_id in worst_ids,
               "t_generate": t["generate"], "t_save": t["save"],
               "t_graph_features": t["graph_features"],
               "t_heuristics": sum(v for kk, v in t.items() if kk.startswith("run_")),
               "t_replay": sum(v for kk, v in t.items() if kk.startswith("replay_")),
               "step_rows": len(out.steps)}
        row["t_total"] = (row["t_generate"] + row["t_save"] + row["t_graph_features"]
                          + row["t_heuristics"] + row["t_replay"])
        for r in out.run_rows:
            row[f"colors_{r['heuristic']}"] = r["n_colors"]
            row[f"t_{r['heuristic']}"] = r["seconds"]
        rows.append(row)
        runs += out.run_rows
        steps.append(out.steps)
        if i % 10 == 0 or row["is_worst_case"]:
            print(f"  {i:4d}/{len(sample)} {spec.graph_id:24s} "
                  f"{row['t_total']:6.2f}s  RSS pico {peak_rss_mb():.0f} MB", flush=True)
    return pd.DataFrame(rows), sizes, pd.concat(steps, ignore_index=True)


def run_rollout_tier(specs, k, rng, worst):
    sample, sizes = stratified_sample(specs, k, rng)
    sample, sampled_ids = with_worst(sample, worst)
    worst_ids = {g.graph_id for g in worst}
    rows, frames = [], []
    print(f"\n[camada B] {len(sample)} grafos medidos de {len(specs)}", flush=True)
    for i, spec in enumerate(sorted(sample, key=lambda g: (g.n, g.model)), 1):
        summary, df = process_rollout(spec)
        summary.update(tier="B", in_sample=spec.graph_id in sampled_ids,
                       is_worst_case=spec.graph_id in worst_ids)
        rows.append(summary)
        frames.append(df)
        print(f"  {i:4d}/{len(sample)} {spec.graph_id:24s} {summary['seconds']:7.2f}s "
              f"oráculo={summary['oracle_colors']}", flush=True)
    return pd.DataFrame(rows), sizes, pd.concat(frames, ignore_index=True)


def parquet_bytes_per_row(df: pd.DataFrame, path: Path) -> float:
    df = df.copy()
    for c in ("graph_id", "heuristic", "suggested_by"):
        if c in df:
            df[c] = df[c].astype("category")
    df.to_parquet(path, compression="zstd", index=False)
    return path.stat().st_size / len(df)



# --------------------------------------------------------------------------- #
HNAMES = {"random": "Aleatória", "welsh_powell": "Welsh-Powell",
          "dsatur": "DSATUR", "rlf": "RLF"}


def heuristic_table(df: pd.DataFrame) -> str:
    cols = [f"colors_{h}" for h in HNAMES]
    best = df[cols].min(axis=1)
    lines = ["| Heurística | Melhor (com empate) | Melhor sozinha | Cores acima da melhor (média) |",
             "|---|---|---|---|"]
    for h, name in HNAMES.items():
        c = df[f"colors_{h}"]
        is_best = c == best
        alone = is_best & ((df[cols] == best.values[:, None]).sum(axis=1) == 1)
        lines.append(f"| {name} | {is_best.mean():.0%} | {alone.mean():.0%} | "
                     f"{(c - best).mean():.2f} |")
    return "\n".join(lines)


def write_report(env, proj, dfA, dfS, dfB, path: Path) -> None:
    A, S, B = proj["A"], proj["S"], proj["B"]
    worst = pd.concat([dfA[dfA.is_worst_case], dfS[dfS.is_worst_case]])
    wl = ["| Grafo | Arestas | Tempo total (s) | Arquivo (MB) | Cores (Aleat./WP/DSATUR/RLF) |",
          "|---|---|---|---|---|"]
    for _, r in worst.iterrows():
        wl.append(f"| {r.graph_id} | {int(r.m):,} | {r.t_total:.2f} | {r.file_bytes/1e6:.2f} | "
                  f"{r.colors_random}/{r.colors_welsh_powell}/{r.colors_dsatur}/{r.colors_rlf} |")
    bw = ["| Grafo | Tempo (s) | Conclusões | Oráculo | DSATUR | RLF | Melhor heurística |",
          "|---|---|---|---|---|---|---|"]
    for _, r in dfB[dfB.is_worst_case].iterrows():
        bh = min(r[f"colors_{h}"] for h in HNAMES)
        bw.append(f"| {r.graph_id} | {r.seconds:.1f} | {r.n_rollouts:,} | {r.oracle_colors} | "
                  f"{r.colors_dsatur} | {r.colors_rlf} | {bh} |")
    best_h = dfB[[f"colors_{h}" for h in HNAMES]].min(axis=1)
    gain_d = dfB.colors_dsatur - dfB.oracle_colors
    gain_b = best_h - dfB.oracle_colors
    gain_r = dfB.colors_rlf - dfB.oracle_colors
    gain_dr = dfB[["colors_dsatur", "colors_rlf"]].min(axis=1) - dfB.oracle_colors
    total_h = A["cpu_hours"] + S["cpu_hours"] + B["cpu_hours"]
    total_mb = (A["graph_files_mb"] + S["graph_files_mb"] + A["steps_parquet_mb"]
                + S["steps_parquet_mb"] + B["rollouts_parquet_mb"])
    txt = f"""# Relatório do piloto

Gerado automaticamente por `scripts/pilot.py`.

## Ambiente

Python {env["python"]}, numpy {env["numpy"]}, pandas {env["pandas"]}, {env["cpus"]} núcleo(s).
Duração do piloto: {env["pilot_wall_minutes"]:.1f} min. Pico de memória (RSS): {env["peak_rss_mb"]:.0f} MB.

## Projeção para a base completa (1 núcleo)

| Camada | Grafos | Medidos | Tempo de CPU estimado | Arquivos de grafos | Linhas por passo | Parquet |
|---|---|---|---|---|---|---|
| A | {A["graphs"]:,} | {A["measured"]} | {A["cpu_hours"]*60:.1f} ± {A["cpu_hours_se"]*60:.1f} min | {A["graph_files_mb"]:.0f} MB | {A["step_rows"]:,} | {A["steps_parquet_mb"]:.0f} MB |
| S | {S["graphs"]:,} | {S["measured"]} | {S["cpu_hours"]*60:.1f} ± {S["cpu_hours_se"]*60:.1f} min | {S["graph_files_mb"]:.0f} MB | {S["step_rows"]:,} | {S["steps_parquet_mb"]:.0f} MB |
| B | {B["graphs"]:,} | {B["measured"]} | {B["cpu_hours"]*60:.1f} ± {B["cpu_hours_se"]*60:.1f} min | (reusa A) | {B["rollout_rows"]:,.0f} | {B["rollouts_parquet_mb"]:.0f} MB |
| **Total** | | | **{total_h*60:.0f} min** | | | **{total_mb:.0f} MB** (grafos + tabelas) |

Os valores "±" são erros-padrão da estimativa estratificada. Bytes por linha em Parquet (zstd):
{env["bytes_per_step_row"]:.1f} (passos) e {env["bytes_per_rollout_row"]:.1f} (rollouts).

## Piores casos — camadas A e S

{chr(10).join(wl)}

## Piores casos — camada B (rollout)

{chr(10).join(bw)}

## Prévia: desempenho das heurísticas na amostra da camada A ({int(dfA.in_sample.sum())} grafos)

Amostra pequena, apenas indicativa; a análise exploratória usará a base completa.

{heuristic_table(dfA[dfA.in_sample])}

## Prévia: coloração oráculo (rollout) na amostra da camada B ({len(dfB)} grafos)

Rollouts completados com DSATUR e com RLF (versão 2). A versão 1, que completava só
com o DSATUR, está em `v1_rollout_somente_dsatur/`.

- Oráculo melhor que o DSATUR em {(gain_d > 0).mean():.0%} dos grafos (ganho médio {gain_d.mean():.2f} cores, máximo {gain_d.max()}).
- Oráculo melhor que o RLF em {(gain_r > 0).mean():.0%} dos grafos (ganho médio {gain_r.mean():.2f} cores, máximo {gain_r.max()}).
- Oráculo pior que min(DSATUR, RLF) em {(gain_dr < 0).mean():.0%} dos grafos (a garantia teórica é 0%).
- Oráculo melhor que a melhor das 4 heurísticas em {(gain_b > 0).mean():.0%} dos grafos
  (ganho médio {gain_b.mean():.2f} cores, máximo {gain_b.max()}); igual em {(gain_b == 0).mean():.0%};
  pior em {(gain_b < 0).mean():.0%}.
"""
    path.write_text(txt, encoding="utf-8")

# --------------------------------------------------------------------------- #
def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--per-stratum-a", type=int, default=2)
    ap.add_argument("--per-stratum-b", type=int, default=1)
    ap.add_argument("--per-stratum-s", type=int, default=1)
    ap.add_argument("--seed", type=int, default=2026)
    args = ap.parse_args()
    rng = random.Random(args.seed)
    OUT.mkdir(parents=True, exist_ok=True)
    TMP.mkdir(parents=True, exist_ok=True)
    t_start = time.perf_counter()

    A_specs, B_specs, S_specs = tier_a(), tier_b(), tier_s()
    worst_a = [GraphSpec("er", 1000, 0.95, 0), GraphSpec("ba", 1000, 300, 0)]
    worst_b = [GraphSpec("er", 500, 0.95, 0), GraphSpec("er", 500, 0.5, 0),
               GraphSpec("ba", 500, 150, 0)]
    worst_s = [GraphSpec("er", 10000, 0.2, 0), GraphSpec("ba", 10000, 20, 0)]

    dfA, sizesA, stepsA = run_graph_tier("A", A_specs, args.per_stratum_a, rng, worst_a)
    dfS, sizesS, stepsS = run_graph_tier("S", S_specs, args.per_stratum_s, rng, worst_s)
    dfB, sizesB, rollB = run_rollout_tier(B_specs, args.per_stratum_b, rng, worst_b)

    # ------------------------------------------------------------ projeções
    bpr_steps = parquet_bytes_per_row(pd.concat([stepsA, stepsS]), OUT / "_steps_sample.parquet")
    bpr_roll = parquet_bytes_per_row(rollB, OUT / "_rollouts_sample.parquet")
    (OUT / "_steps_sample.parquet").unlink()
    (OUT / "_rollouts_sample.parquet").unlink()

    proj = {}
    for name, df, sizes, specs in (("A", dfA, sizesA, A_specs), ("S", dfS, sizesS, S_specs)):
        t, se = stratified_total(df, "t_total", sizes)
        fb, fse = stratified_total(df, "file_bytes", sizes)
        rows_total = 4 * sum(g.n for g in specs)
        proj[name] = {"graphs": len(specs), "measured": len(df),
                      "cpu_hours": t / 3600, "cpu_hours_se": se / 3600,
                      "graph_files_mb": fb / 1e6, "graph_files_mb_se": fse / 1e6,
                      "step_rows": rows_total,
                      "steps_parquet_mb": rows_total * bpr_steps / 1e6}
    t, se = stratified_total(dfB, "seconds", sizesB)
    r, rse = stratified_total(dfB, "rows", sizesB)
    proj["B"] = {"graphs": len(B_specs), "measured": len(dfB),
                 "cpu_hours": t / 3600, "cpu_hours_se": se / 3600,
                 "rollout_rows": r, "rollout_rows_se": rse,
                 "rollouts_parquet_mb": r * bpr_roll / 1e6}

    env = {"python": platform.python_version(), "numpy": np.__version__,
           "pandas": pd.__version__, "platform": platform.platform(),
           "cpus": __import__("os").cpu_count(),
           "peak_rss_mb": peak_rss_mb(),
           "pilot_wall_minutes": (time.perf_counter() - t_start) / 60,
           "bytes_per_step_row": bpr_steps, "bytes_per_rollout_row": bpr_roll}

    dfA.to_csv(OUT / "pilot_tier_A.csv", index=False)
    dfS.to_csv(OUT / "pilot_tier_S.csv", index=False)
    dfB.to_csv(OUT / "pilot_tier_B.csv", index=False)
    with open(OUT / "summary.json", "w") as f:
        json.dump({"environment": env, "projection": proj,
                   "args": vars(args)}, f, indent=2, ensure_ascii=False)
    write_report(env, proj, dfA, dfS, dfB, OUT / "RELATORIO_PILOTO.md")
    for p in TMP.glob("*.npz"):
        p.unlink()
    TMP.rmdir()

    print("\n" + json.dumps({"environment": env, "projection": proj},
                            indent=2, ensure_ascii=False))


if __name__ == "__main__":
    main()
