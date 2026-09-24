"""Definição das camadas da base de dados (ver docs/ESPECIFICACAO_BASE.md).

Camada A (principal): ER e BA, n = 50..1000 (passo 50), 5 sementes.
Camada B (rollout):   subconjunto de A com n <= 500 e sementes 0..2.
Camada S (escala):    n em {2000, 5000, 10000}, ER p em {0.05, 0.1, 0.2},
                      BA m em {2, 5, 10, 20}, 3 sementes.
"""
from __future__ import annotations

from .graphs import GraphSpec

P_VALUES = [round(0.05 * i, 2) for i in range(1, 20)]          # 0.05 .. 0.95
N_VALUES_A = list(range(50, 1001, 50))                          # 50 .. 1000
BA_FIXED_M = (2, 3, 5, 8)
BA_FRACTIONS = (0.02, 0.05, 0.10, 0.20, 0.30)


def ba_m_values(n: int) -> list[int]:
    """Valores de m para BA: fixos {2,3,5,8} ∪ {2,5,10,20,30% de n}, sem duplicatas."""
    ms = set(BA_FIXED_M) | {max(2, round(f * n)) for f in BA_FRACTIONS}
    return sorted(m for m in ms if 2 <= m < n)


def tier_a() -> list[GraphSpec]:
    specs = [GraphSpec("er", n, p, s)
             for n in N_VALUES_A for p in P_VALUES for s in range(5)]
    specs += [GraphSpec("ba", n, m, s)
              for n in N_VALUES_A for m in ba_m_values(n) for s in range(5)]
    return specs


def tier_b() -> list[GraphSpec]:
    return [g for g in tier_a() if g.n <= 500 and g.seed <= 2]


def tier_s() -> list[GraphSpec]:
    ns = (2000, 5000, 10000)
    specs = [GraphSpec("er", n, p, s) for n in ns for p in (0.05, 0.1, 0.2)
             for s in range(3)]
    specs += [GraphSpec("ba", n, m, s) for n in ns for m in (2, 5, 10, 20)
              for s in range(3)]
    return specs


TIERS = {"A": tier_a, "B": tier_b, "S": tier_s}
