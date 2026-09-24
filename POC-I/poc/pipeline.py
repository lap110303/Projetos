"""Processamento de um grafo da base: funções reutilizadas pelo piloto e,
depois, pelos scripts de construção da base completa.

process_graph   -> camadas A e S: gera, salva, atributos globais, 4 heurísticas
                   e trajetórias (atributos por passo)
process_rollout -> camada B: trajetória guiada por rollout (rótulos por passo)
"""
from __future__ import annotations

import time
from dataclasses import dataclass
from pathlib import Path

import pandas as pd

from .features import graph_features, replay
from .graphs import GraphSpec, generate, stable_seed
from .heuristics import HEURISTICS, is_valid_coloring, run_heuristic
from .rollout import rollout_trajectory
from .storage import save_graph


@dataclass
class GraphOutput:
    graph_row: dict            # 1 linha para graphs.parquet
    run_rows: list[dict]       # 4 linhas para runs.parquet
    steps: pd.DataFrame        # 4n linhas para steps.parquet
    timings: dict              # tempos por etapa (s)


def _spec_cols(spec: GraphSpec) -> dict:
    return {"graph_id": spec.graph_id, "model": spec.model, "n": spec.n,
            "param": spec.param, "seed": spec.seed}


def process_graph(spec: GraphSpec, graph_dir: str | Path | None = None,
                  validate: bool = True) -> GraphOutput:
    t = {}
    t0 = time.perf_counter()
    A = generate(spec)
    t["generate"] = time.perf_counter() - t0

    file_bytes = None
    if graph_dir is not None:
        t0 = time.perf_counter()
        file_bytes = save_graph(Path(graph_dir) / f"{spec.graph_id}.npz", A)
        t["save"] = time.perf_counter() - t0

    t0 = time.perf_counter()
    gfeat = graph_features(A)
    t["graph_features"] = time.perf_counter() - t0

    run_rows, step_frames = [], []
    rand_seed = stable_seed(spec.graph_id, "random-order")
    for h in HEURISTICS:
        r = run_heuristic(h, A, seed=rand_seed)
        t[f"run_{h}"] = r.seconds
        if validate and not is_valid_coloring(A, r.colors):
            raise RuntimeError(f"coloração inválida: {spec.graph_id} / {h}")
        t0 = time.perf_counter()
        cols, colors = replay(A, r.order)
        t[f"replay_{h}"] = time.perf_counter() - t0
        if validate and not (colors == r.colors).all():
            raise RuntimeError(f"replay divergente: {spec.graph_id} / {h}")
        df = pd.DataFrame(cols)
        df.insert(0, "heuristic", h)
        df.insert(0, "graph_id", spec.graph_id)
        step_frames.append(df)
        run_rows.append({"graph_id": spec.graph_id, "heuristic": h,
                         "n_colors": r.n_colors, "seconds": r.seconds})

    graph_row = {**_spec_cols(spec), **gfeat}
    if file_bytes is not None:
        graph_row["file_bytes"] = file_bytes
    return GraphOutput(graph_row, run_rows, pd.concat(step_frames, ignore_index=True), t)


def process_rollout(spec: GraphSpec) -> tuple[dict, pd.DataFrame]:
    A = generate(spec)
    r = rollout_trajectory(A, seed=stable_seed(spec.graph_id, "random-order"))
    if not is_valid_coloring(A, r.colors):
        raise RuntimeError(f"coloração oráculo inválida: {spec.graph_id}")
    df = pd.DataFrame(r.rows)
    df.insert(0, "graph_id", spec.graph_id)
    summary = {**_spec_cols(spec), "oracle_colors": r.n_colors,
               "n_rollouts": r.n_rollouts, "rows": len(df),
               "seconds": r.seconds}
    for h in HEURISTICS:          # referência: as 4 heurísticas no mesmo grafo
        summary[f"colors_{h}"] = run_heuristic(
            h, A, seed=stable_seed(spec.graph_id, "random-order")).n_colors
    return summary, df
