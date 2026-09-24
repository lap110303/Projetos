"""Estado parcial de uma construção gulosa de coloração.

Todas as heurísticas deste projeto seguem o mesmo esquema:
    enquanto houver vértice sem cor:
        v = regra_de_escolha(estado)
        atribuir a v a MENOR cor não usada por seus vizinhos
A única diferença entre elas é a regra de escolha. Isso permite:
  * misturar regras ao longo da construção (base da camada B / rollouts);
  * extrair os mesmos atributos por passo para todas as heurísticas.

Estruturas mantidas incrementalmente (todas vetores numpy):
  color[v]      cor do vértice (-1 se não colorido)
  uncolored[v]  True se v ainda não tem cor
  has[c, v]     True se algum vizinho de v tem cor c   (matriz cores x vértices)
  sat[v]        grau de saturação = nº de cores distintas na vizinhança de v
  udeg[v]       grau de v no subgrafo induzido pelos vértices sem cor
"""
from __future__ import annotations

import numpy as np


class ColoringState:
    __slots__ = ("A", "n", "deg", "color", "uncolored", "has", "sat", "udeg",
                 "n_colors", "step")

    def __init__(self, A: np.ndarray):
        self.A = A
        self.n = n = A.shape[0]
        self.deg = A.sum(axis=1).astype(np.int32)
        self.color = np.full(n, -1, dtype=np.int32)
        self.uncolored = np.ones(n, dtype=bool)
        max_colors = int(self.deg.max(initial=0)) + 2   # guloso usa <= Δ+1 cores
        self.has = np.zeros((max_colors, n), dtype=bool)
        self.sat = np.zeros(n, dtype=np.int32)
        self.udeg = self.deg.copy()
        self.n_colors = 0
        self.step = 0

    # ------------------------------------------------------------------ #
    def smallest_available(self, v: int) -> int:
        """Menor cor não usada pelos vizinhos de v (argmin de bool = 1º False)."""
        return int(np.argmin(self.has[:, v]))

    def assign(self, v: int, c: int | None = None) -> int:
        """Colore v (por padrão com a menor cor disponível) e atualiza o estado."""
        if not self.uncolored[v]:
            raise ValueError(f"vértice {v} já está colorido")
        if c is None:
            c = self.smallest_available(v)
        elif self.has[c, v]:
            raise ValueError(f"cor {c} conflita com vizinho de {v}")
        nb = self.A[v]
        self.color[v] = c
        self.uncolored[v] = False
        newly = nb & self.uncolored & ~self.has[c]
        self.sat[newly] += 1
        self.has[c, nb] = True
        self.udeg[nb] -= 1
        if c + 1 > self.n_colors:
            self.n_colors = c + 1
        self.step += 1
        return c

    @property
    def done(self) -> bool:
        return self.step == self.n

    def copy(self) -> "ColoringState":
        s = ColoringState.__new__(ColoringState)
        s.A, s.n, s.deg = self.A, self.n, self.deg          # compartilhados
        s.color = self.color.copy()
        s.uncolored = self.uncolored.copy()
        s.has = self.has.copy()
        s.sat = self.sat.copy()
        s.udeg = self.udeg.copy()
        s.n_colors = self.n_colors
        s.step = self.step
        return s
