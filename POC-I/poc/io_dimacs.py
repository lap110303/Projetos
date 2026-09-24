"""Leitura e escrita no formato DIMACS de coloração (.col).

Formato:  c <comentário>
          p edge <n> <m>
          e <u> <v>        (vértices numerados a partir de 1)
"""
from __future__ import annotations

from pathlib import Path

import numpy as np

from .graphs import from_edges


def read_dimacs(path: str | Path) -> np.ndarray:
    n = None
    edges = []
    with open(path, "r", errors="replace") as f:
        for line in f:
            parts = line.split()
            if not parts or parts[0] == "c":
                continue
            if parts[0] == "p":
                n = int(parts[2])
            elif parts[0] == "e":
                edges.append((int(parts[1]) - 1, int(parts[2]) - 1))
    if n is None:
        raise ValueError(f"{path}: linha 'p' ausente")
    e = np.array(edges, dtype=np.int64).reshape(-1, 2)
    return from_edges(n, e)          # remove duplicatas e laços automaticamente


def write_dimacs(path: str | Path, A: np.ndarray, comment: str = "") -> None:
    iu, ju = np.nonzero(np.triu(A, 1))
    with open(path, "w") as f:
        if comment:
            f.write(f"c {comment}\n")
        f.write(f"p edge {A.shape[0]} {len(iu)}\n")
        for u, v in zip(iu + 1, ju + 1):
            f.write(f"e {u} {v}\n")
