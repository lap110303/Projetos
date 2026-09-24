"""Número cromático exato por backtracking — só para grafos pequenos (testes)."""
from __future__ import annotations

import numpy as np


def chromatic_number(A: np.ndarray) -> int:
    n = A.shape[0]
    if n == 0:
        return 0
    order = list(np.argsort(-A.sum(axis=1), kind="stable"))
    nbrs = [np.flatnonzero(A[v]).tolist() for v in range(n)]

    def colorable(k: int) -> bool:
        color = [-1] * n

        def bt(i: int, used: int) -> bool:
            if i == n:
                return True
            v = order[i]
            forbidden = {color[u] for u in nbrs[v]}
            for c in range(min(used + 1, k)):      # quebra de simetria
                if c not in forbidden:
                    color[v] = c
                    if bt(i + 1, max(used, c + 1)):
                        return True
            color[v] = -1
            return False

        return bt(0, 0)

    k = 1
    while not colorable(k):
        k += 1
    return k
