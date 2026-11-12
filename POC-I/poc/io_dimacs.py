"""Leitura e escrita no formato DIMACS de coloração (.col) e no binário (.col.b).

Texto:    c <comentário>
          p edge <n> <m>
          e <u> <v>        (vértices numerados a partir de 1)

Binário (DIMACS Challenge II, README.binformat / bin2asc.c):
  1ª linha: tamanho L do preâmbulo; em seguida L caracteres de preâmbulo em texto
  (comentários e a linha "p"); depois, para cada vértice i = 0..n-1, a linha i do
  triângulo inferior da matriz de adjacência (colunas j = 0..i) em (i + 8) // 8
  bytes, bit mais significativo primeiro (coluna j = bit 7 - j % 8 do byte j // 8).
"""
from __future__ import annotations

from pathlib import Path

import numpy as np

from .graphs import from_edges


def read_dimacs(path: str | Path) -> np.ndarray:
    """Lê .col (texto) ou .b (binário, pela extensão) e devolve a matriz de adjacência."""
    if str(path).endswith(".b"):
        return read_dimacs_binary(path)
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


def read_dimacs_binary(path: str | Path) -> np.ndarray:
    data = Path(path).read_bytes()
    nl = data.index(b"\n")
    length = int(data[:nl])
    preamble = data[nl + 1:nl + 1 + length].decode("ascii", errors="replace")
    n = None
    for line in preamble.splitlines():
        parts = line.split()
        if parts and parts[0] == "p":
            n = int(parts[2])
    if n is None:
        raise ValueError(f"{path}: linha 'p' ausente no preâmbulo")
    pos = nl + 1 + length
    need = sum((i + 8) // 8 for i in range(n))
    if len(data) - pos < need:
        raise ValueError(f"{path}: bloco binário truncado ({len(data) - pos} de {need} bytes)")
    A = np.zeros((n, n), dtype=bool)
    for i in range(n):
        nb = (i + 8) // 8
        A[i, :i + 1] = np.unpackbits(np.frombuffer(data, np.uint8, nb, pos))[:i + 1]
        pos += nb
    np.fill_diagonal(A, False)
    A |= A.T
    return A


def write_dimacs_binary(path: str | Path, A: np.ndarray, comment: str = "") -> None:
    """Grava no formato binário (usado nos testes do leitor)."""
    n = A.shape[0]
    pre = (f"c {comment}\n" if comment else "") + f"p edge {n} {int(np.triu(A, 1).sum())}\n"
    out = bytearray(f"{len(pre)}\n{pre}".encode("ascii"))
    low = np.tril(A, -1)
    for i in range(n):
        out += np.packbits(low[i, :i + 1]).tobytes()      # (i + 8) // 8 bytes
    Path(path).write_bytes(bytes(out))


def write_dimacs(path: str | Path, A: np.ndarray, comment: str = "") -> None:
    iu, ju = np.nonzero(np.triu(A, 1))
    with open(path, "w") as f:
        if comment:
            f.write(f"c {comment}\n")
        f.write(f"p edge {A.shape[0]} {len(iu)}\n")
        for u, v in zip(iu + 1, ju + 1):
            f.write(f"e {u} {v}\n")
