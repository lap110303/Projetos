"""Construção da base completa (camadas A, S e B) — ver docs/ESPECIFICACAO_BASE.md.

Reutiliza poc/pipeline.py: process_graph (camadas A e S) e process_rollout (camada B).

Layout de saída (padrão: data/):
    graphs/{graph_id}.npz           grafos das camadas A e S (B reusa os de A)
    A/graphs/part-NNNNN.parquet     1 linha por grafo  (tabela-índice da camada)
    A/runs/part-NNNNN.parquet       1 linha por (grafo, heurística)
    A/steps/part-NNNNN.parquet      1 linha por (grafo, heurística, passo)
    S/...                           mesmas tabelas da camada A
    B/oracle/part-NNNNN.parquet     1 linha por grafo  (tabela-índice): oráculo e heurísticas
    B/rollouts/part-NNNNN.parquet   1 linha por (grafo, passo, candidato)
    build_log.txt                   log de progresso (acumulado entre execuções)
    manifest.json                   versões, argumentos e contagens por camada

Cada tabela é um diretório de partes; pd.read_parquet("data/A/steps") lê todas.

Retomada: a tabela-índice de cada camada é a fonte da verdade sobre o que já foi
processado. Cada lote é gravado com o mesmo número de parte em todas as tabelas,
a tabela-índice por último, e cada arquivo é escrito como .tmp e depois renomeado.
Ao iniciar, partes sem a parte correspondente no índice (interrupção no meio da
gravação de um lote) são apagadas, e os grafos já presentes no índice são pulados.
Um Ctrl+C grava o lote em memória antes de sair.

Validação: o pipeline valida toda coloração (is_valid_coloring) e confere o replay
de cada heurística; aqui ainda se conferem contagens de linhas, o número de cores
reconstruído pelas trajetórias e, na camada B, um candidato escolhido por passo e a
garantia oráculo <= min(DSATUR, RLF). Grafos que falham são registrados no log, não
são gravados e serão tentados de novo na próxima execução.

Uso (a partir da pasta POC-I):
    python scripts/build_dataset.py                       # camadas A, S e B
    python scripts/build_dataset.py --tiers A --workers 6
    python scripts/build_dataset.py --tiers B --limit 20  # teste rápido
"""
from __future__ import annotations

import os

# Um processo por núcleo: impede que cada worker abra vários threads de BLAS.
for _var in ("OMP_NUM_THREADS", "OPENBLAS_NUM_THREADS", "MKL_NUM_THREADS"):
    os.environ.setdefault(_var, "1")

import argparse               # noqa: E402
import json                   # noqa: E402
import logging                # noqa: E402
import multiprocessing as mp  # noqa: E402
import platform               # noqa: E402
import re                     # noqa: E402
import signal                 # noqa: E402
import sys                    # noqa: E402
import time                   # noqa: E402
import traceback              # noqa: E402
from datetime import datetime  # noqa: E402
from pathlib import Path      # noqa: E402

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

import numpy as np            # noqa: E402
import pandas as pd           # noqa: E402

from poc.graphs import GraphSpec                          # noqa: E402
from poc.grids import TIERS                               # noqa: E402
from poc.heuristics import HEURISTICS                     # noqa: E402
from poc.pipeline import process_graph, process_rollout   # noqa: E402

# camada -> (tabela-índice, demais tabelas)
TABLES = {"A": ("graphs", ("runs", "steps")),
          "S": ("graphs", ("runs", "steps")),
          "B": ("oracle", ("rollouts",))}
PART_RE = re.compile(r"^part-(\d{5})\.parquet$")

log = logging.getLogger("build_dataset")


# --------------------------------------------------------------------------- #
# Trabalho de um grafo (executado nos processos filhos)
# --------------------------------------------------------------------------- #
def _init_worker() -> None:
    # Ctrl+C é tratado só pelo processo principal, que grava o lote e encerra o pool.
    signal.signal(signal.SIGINT, signal.SIG_IGN)


def work_graph(args: tuple[GraphSpec, str]) -> tuple[str, dict | None, str | None]:
    """Camadas A e S. Retorna (graph_id, {tabela: DataFrame}, erro)."""
    spec, graph_dir = args
    try:
        out = process_graph(spec, graph_dir=graph_dir, validate=True)
        n, t = spec.n, out.timings
        if len(out.run_rows) != len(HEURISTICS) or len(out.steps) != len(HEURISTICS) * n:
            raise RuntimeError(f"contagem de linhas inesperada: {len(out.run_rows)} "
                               f"execuções, {len(out.steps)} passos")
        for r in out.run_rows:                   # trajetória reconstrói o nº de cores
            last = out.steps[out.steps["heuristic"] == r["heuristic"]].iloc[-1]
            if last["n_colors_before"] + last["opens_new_color"] != r["n_colors"]:
                raise RuntimeError(f"trajetória inconsistente: {r['heuristic']}")
        row = {**out.graph_row,
               "t_generate": t["generate"], "t_save": t.get("save", 0.0),
               "t_graph_features": t["graph_features"],
               "t_heuristics": sum(v for k, v in t.items() if k.startswith("run_")),
               "t_replay": sum(v for k, v in t.items() if k.startswith("replay_"))}
        return spec.graph_id, {"graphs": pd.DataFrame([row]),
                               "runs": pd.DataFrame(out.run_rows),
                               "steps": out.steps}, None
    except Exception:
        return spec.graph_id, None, traceback.format_exc()


def work_rollout(args: tuple[GraphSpec, str]) -> tuple[str, dict | None, str | None]:
    """Camada B. Retorna (graph_id, {tabela: DataFrame}, erro)."""
    spec, _ = args
    try:
        summary, df = process_rollout(spec)
        chosen = df.loc[df["chosen"] == 1, "step"]
        if len(chosen) != spec.n or chosen.nunique() != spec.n:
            raise RuntimeError("esperado exatamente 1 candidato escolhido por passo")
        if (df.loc[df["chosen"] == 1, "label"] != 1).any():
            raise RuntimeError("candidato escolhido com rótulo 0")
        base = min(summary["colors_dsatur"], summary["colors_rlf"])
        if summary["oracle_colors"] > base:
            raise RuntimeError(f"oráculo ({summary['oracle_colors']}) pior que "
                               f"min(DSATUR, RLF) ({base})")
        return spec.graph_id, {"oracle": pd.DataFrame([summary]), "rollouts": df}, None
    except Exception:
        return spec.graph_id, None, traceback.format_exc()


WORKERS = {"A": work_graph, "S": work_graph, "B": work_rollout}


# --------------------------------------------------------------------------- #
# Partes em disco
# --------------------------------------------------------------------------- #
def _parts(d: Path) -> dict[int, Path]:
    if not d.is_dir():
        return {}
    return {int(m.group(1)): p for p in d.iterdir() if (m := PART_RE.match(p.name))}


def _replace(src: Path, dst: Path, tries: int = 10) -> None:
    """os.replace com novas tentativas (antivírus/OneDrive podem travar o arquivo)."""
    for i in range(tries):
        try:
            os.replace(src, dst)
            return
        except PermissionError:
            if i == tries - 1:
                raise
            time.sleep(0.5 * (i + 1))


def clean_orphans(tdir: Path, tier: str) -> None:
    """Apaga .tmp e partes que não têm a parte correspondente na tabela-índice."""
    index_tbl, others = TABLES[tier]
    indexed = set(_parts(tdir / index_tbl))
    for t in (index_tbl, *others):
        d = tdir / t
        if not d.is_dir():
            continue
        for p in d.glob("*.tmp"):
            p.unlink()
            log.warning("  removido temporário incompleto %s", p.relative_to(tdir.parent))
        if t == index_tbl:
            continue
        for k, p in _parts(d).items():
            if k not in indexed:
                p.unlink()
                log.warning("  removida parte órfã %s (lote interrompido)",
                            p.relative_to(tdir.parent))


def done_ids(tdir: Path, tier: str) -> set[str]:
    """Ids já gravados na tabela-índice. Falha se houver duplicatas."""
    ids: list[str] = []
    for _, p in sorted(_parts(tdir / TABLES[tier][0]).items()):
        ids += pd.read_parquet(p, columns=["graph_id"])["graph_id"].tolist()
    if len(ids) != len(set(ids)):
        dup = pd.Series(ids)[pd.Series(ids).duplicated()].unique()[:5].tolist()
        raise RuntimeError(f"camada {tier}: grafos duplicados na tabela-índice: {dup}")
    return set(ids)


def next_part(tdir: Path, tier: str) -> int:
    ks = [k for t in (TABLES[tier][0], *TABLES[tier][1]) for k in _parts(tdir / t)]
    return max(ks, default=-1) + 1


def write_batch(tdir: Path, tier: str, part: int, buf: dict[str, list[pd.DataFrame]]) -> None:
    index_tbl, others = TABLES[tier]
    for t in (*others, index_tbl):                   # índice por último
        d = tdir / t
        d.mkdir(parents=True, exist_ok=True)
        path = d / f"part-{part:05d}.parquet"
        tmp = d / f"part-{part:05d}.parquet.tmp"
        pd.concat(buf[t], ignore_index=True).to_parquet(tmp, compression="zstd", index=False)
        _replace(tmp, path)


# --------------------------------------------------------------------------- #
# Construção de uma camada
# --------------------------------------------------------------------------- #
def _cost_key(g: GraphSpec) -> tuple[int, float]:
    """Ordem decrescente de custo (grafos grandes e densos primeiro): equilibra
    a carga entre os processos e expõe cedo os casos de memória mais altos."""
    density = g.param if g.model == "er" else 2 * g.param / g.n
    return g.n, density


def _fmt_time(s: float) -> str:
    s = int(s)
    return f"{s // 3600}h{s % 3600 // 60:02d}m" if s >= 3600 else f"{s // 60}m{s % 60:02d}s"


def build_tier(tier: str, specs: list[GraphSpec], out: Path, workers: int,
               batch_graphs: int = 100, batch_rows: int = 2_000_000,
               limit: int | None = None) -> dict:
    """Processa os grafos pendentes da camada. Retorna contagens para o manifesto."""
    tdir = out / tier
    graph_dir = str(out / "graphs")
    index_tbl, others = TABLES[tier]
    clean_orphans(tdir, tier)
    done = done_ids(tdir, tier)
    expected = {g.graph_id for g in specs}
    extra = done - expected
    if extra:
        log.warning("camada %s: %d grafos gravados fora da grade atual (ex.: %s)",
                    tier, len(extra), sorted(extra)[0])
    todo = sorted((g for g in specs if g.graph_id not in done), key=_cost_key, reverse=True)
    if limit is not None:
        todo = todo[:limit]
    log.info("[camada %s] %d grafos na grade, %d já processados, %d a processar "
             "(%d processo(s))", tier, len(specs), len(expected & done), len(todo), workers)

    failed: list[str] = []
    n_ok = 0
    if todo:
        buf: dict[str, list[pd.DataFrame]] = {t: [] for t in (index_tbl, *others)}
        buf_graphs = buf_rows = 0
        part = next_part(tdir, tier)

        def flush() -> None:
            nonlocal buf_graphs, buf_rows, part
            if buf_graphs == 0:
                return
            write_batch(tdir, tier, part, buf)
            log.info("  lote gravado: parte %05d (%d grafos, %d linhas)",
                     part, buf_graphs, buf_rows)
            for t in buf:
                buf[t].clear()
            buf_graphs = buf_rows = 0
            part += 1

        func = WORKERS[tier]
        tasks = [(g, graph_dir) for g in todo]
        pool = mp.Pool(workers, initializer=_init_worker) if workers > 1 else None
        results = (pool.imap_unordered(func, tasks, chunksize=1) if pool
                   else map(func, tasks))
        t0 = last_log = time.perf_counter()
        every = max(1, len(todo) // 50)
        try:
            for i, (gid, frames, err) in enumerate(results, 1):
                if err is not None:
                    failed.append(gid)
                    log.error("  FALHA em %s:\n%s", gid, err.rstrip())
                else:
                    n_ok += 1
                    for t, df in frames.items():
                        buf[t].append(df)
                    buf_graphs += 1
                    buf_rows += sum(len(df) for df in frames.values())
                    if buf_graphs >= batch_graphs or buf_rows >= batch_rows:
                        flush()
                now = time.perf_counter()
                if i % every == 0 or i == len(todo) or now - last_log > 60:
                    last_log = now
                    el = now - t0
                    log.info("  %5d/%d (%3.0f%%)  decorrido %s  restante ~%s  último: %s",
                             i, len(todo), 100 * i / len(todo), _fmt_time(el),
                             _fmt_time(el / i * (len(todo) - i)), gid)
        except KeyboardInterrupt:
            log.warning("interrompido pelo usuário; gravando o lote em memória")
            raise
        finally:
            flush()
            if pool is not None:
                pool.terminate()
                pool.join()

    done = done_ids(tdir, tier)
    missing = expected - done
    if failed:
        log.error("[camada %s] %d falha(s): %s", tier, len(failed), ", ".join(failed[:10]))
    log.info("[camada %s] %d/%d grafos gravados%s", tier, len(expected & done), len(expected),
             "" if missing else " — camada completa")
    return {"expected": len(expected), "done": len(expected & done),
            "missing": len(missing), "failed_last_run": failed,
            "processed_last_run": n_ok}


# --------------------------------------------------------------------------- #
def setup_logging(out: Path) -> None:
    out.mkdir(parents=True, exist_ok=True)
    log.setLevel(logging.INFO)
    log.handlers.clear()
    fmt = logging.Formatter("%(asctime)s %(levelname)-7s %(message)s", "%Y-%m-%d %H:%M:%S")
    for h in (logging.StreamHandler(sys.stdout),
              logging.FileHandler(out / "build_log.txt", encoding="utf-8")):
        h.setFormatter(fmt)
        log.addHandler(h)


def write_manifest(out: Path, args: argparse.Namespace, stats: dict) -> None:
    import networkx
    import pyarrow
    path = out / "manifest.json"
    man = json.loads(path.read_text(encoding="utf-8")) if path.exists() else {}
    man.setdefault("tiers", {}).update(
        {t: {**s, "updated": datetime.now().isoformat(timespec="seconds")}
         for t, s in stats.items()})
    man["environment"] = {"python": platform.python_version(), "numpy": np.__version__,
                          "pandas": pd.__version__, "networkx": networkx.__version__,
                          "pyarrow": pyarrow.__version__, "platform": platform.platform()}
    man["last_args"] = {k: (str(v) if isinstance(v, Path) else v)
                        for k, v in vars(args).items()}
    tmp = path.with_suffix(".json.tmp")
    tmp.write_text(json.dumps(man, indent=2, ensure_ascii=False), encoding="utf-8")
    _replace(tmp, path)


def main(argv: list[str] | None = None) -> int:
    cpus = os.cpu_count() or 1
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--tiers", nargs="+", choices=list(TABLES), default=["A", "S", "B"])
    ap.add_argument("--out", type=Path, default=ROOT / "data")
    ap.add_argument("--workers", type=int, default=max(1, cpus // 2),
                    help="processos em paralelo (padrão: metade dos núcleos lógicos)")
    ap.add_argument("--workers-s", type=int, default=None,
                    help="processos na camada S, cujo pior grafo usa ~560 MB "
                         "(padrão: min(--workers, 4))")
    ap.add_argument("--batch-graphs", type=int, default=100,
                    help="grafos por parte Parquet")
    ap.add_argument("--batch-rows", type=int, default=2_000_000,
                    help="linhas acumuladas que forçam a gravação do lote")
    ap.add_argument("--limit", type=int, default=None,
                    help="processa no máximo N grafos pendentes por camada (testes)")
    args = ap.parse_args(argv)

    if hasattr(sys.stdout, "reconfigure"):
        sys.stdout.reconfigure(encoding="utf-8", errors="replace")
    setup_logging(args.out)
    log.info("=== build_dataset: camadas %s -> %s", " ".join(args.tiers), args.out)
    stats = {}
    try:
        for tier in args.tiers:
            w = args.workers
            if tier == "S":
                w = args.workers_s if args.workers_s is not None else min(w, 4)
            stats[tier] = build_tier(tier, TIERS[tier](), args.out, max(1, w),
                                     args.batch_graphs, args.batch_rows, args.limit)
    except KeyboardInterrupt:
        return 130
    finally:
        if stats:
            write_manifest(args.out, args, stats)
    return 1 if any(s["failed_last_run"] for s in stats.values()) else 0


if __name__ == "__main__":
    sys.exit(main())
