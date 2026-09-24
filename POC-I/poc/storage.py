"""Armazenamento compacto de grafos em .npz comprimido.

Escolhe automaticamente o formato menor:
  * "bits":  triângulo superior da matriz de adjacência em bits (n(n-1)/16 bytes)
  * "edges": lista de arestas (uint16 se n <= 65535, 4 bytes por aresta)
"""
from __future__ import annotations

from pathlib import Path

import numpy as np


def _upper_bits(A: np.ndarray) -> np.ndarray:
    n = A.shape[0]
    return np.packbits(np.concatenate([A[i, i + 1:] for i in range(n - 1)]))


def save_graph(path: str | Path, A: np.ndarray, **meta) -> int:
    """Salva o grafo e retorna o tamanho do arquivo em bytes."""
    n = A.shape[0]
    m = int(A.sum()) // 2
    bits_bytes = n * (n - 1) // 16
    dtype = np.uint16 if n <= 65535 else np.uint32
    edge_bytes = m * 2 * np.dtype(dtype).itemsize
    path = Path(path)
    path.parent.mkdir(parents=True, exist_ok=True)
    if edge_bytes < bits_bytes:
        iu, ju = np.nonzero(np.triu(A, 1))
        data, fmt = np.stack([iu, ju], axis=1).astype(dtype), "edges"
    else:
        data, fmt = _upper_bits(A), "bits"
    np.savez_compressed(path, n=n, fmt=fmt, data=data,
                        **{f"meta_{k}": v for k, v in meta.items()})
    return path.stat().st_size


def load_graph(path: str | Path) -> np.ndarray:
    z = np.load(path, allow_pickle=False)
    n, fmt, data = int(z["n"]), str(z["fmt"]), z["data"]
    A = np.zeros((n, n), dtype=bool)
    if fmt == "edges":
        e = data.astype(np.int64)
        A[e[:, 0], e[:, 1]] = True
    else:
        bits = np.unpackbits(data)
        pos = 0
        for i in range(n - 1):
            L = n - i - 1
            A[i, i + 1:] = bits[pos:pos + L]
            pos += L
    A |= A.T
    return A
