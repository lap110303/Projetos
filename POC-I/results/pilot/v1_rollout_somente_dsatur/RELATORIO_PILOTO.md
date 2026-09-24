# Relatório do piloto

Gerado automaticamente por `scripts/pilot.py`.

## Ambiente

Python 3.12.3, numpy 2.4.4, pandas 3.0.2, 1 núcleo(s).
Duração do piloto: 4.9 min. Pico de memória (RSS): 558 MB.

## Projeção para a base completa (1 núcleo)

| Camada | Grafos | Medidos | Tempo de CPU estimado | Arquivos de grafos | Linhas por passo | Parquet |
|---|---|---|---|---|---|---|
| A | 2,755 | 82 | 5.8 ± 0.2 min | 43 MB | 5,854,000 | 41 MB |
| S | 63 | 19 | 7.5 ± 0.1 min | 60 MB | 1,428,000 | 10 MB |
| B | 813 | 43 | 38.8 ± 1.1 min | (reusa A) | 861,525 | 6 MB |
| **Total** | | | **52 min** | | | **160 MB** (grafos + tabelas) |

Os valores "±" são erros-padrão da estimativa estratificada. Bytes por linha em Parquet (zstd):
7.1 (passos) e 7.4 (rollouts).

## Piores casos — camadas A e S

| Grafo | Arestas | Tempo total (s) | Arquivo (MB) | Cores (Aleat./WP/DSATUR/RLF) |
|---|---|---|---|---|
| ba_n1000_m300_s0 | 210,000 | 0.65 | 0.05 | 130/98/94/97 |
| er_n1000_p0.95_s0 | 474,645 | 0.35 | 0.02 | 383/386/372/334 |
| ba_n10000_m20_s0 | 199,600 | 6.18 | 0.49 | 26/15/13/14 |
| er_n10000_p0.20_s0 | 9,997,369 | 32.32 | 4.85 | 342/336/322/302 |

## Piores casos — camada B (rollout)

| Grafo | Tempo (s) | Rollouts | Oráculo | DSATUR | Melhor heurística |
|---|---|---|---|---|---|
| ba_n500_m150_s0 | 8.2 | 1,428 | 55 | 55 | 55 |
| er_n500_p0.95_s0 | 7.5 | 1,434 | 193 | 204 | 183 |
| er_n500_p0.50_s0 | 9.7 | 1,466 | 63 | 66 | 60 |

## Prévia: desempenho das heurísticas na amostra da camada A (80 grafos)

Amostra pequena, apenas indicativa; a análise exploratória usará a base completa.

| Heurística | Melhor (com empate) | Melhor sozinha | Cores acima da melhor (média) |
|---|---|---|---|
| Aleatória | 0% | 0% | 12.74 |
| Welsh-Powell | 5% | 0% | 6.92 |
| DSATUR | 57% | 42% | 3.20 |
| RLF | 56% | 42% | 0.62 |

## Prévia: coloração oráculo (rollout) na amostra da camada B (43 grafos)

- Oráculo melhor que o DSATUR em 51% dos grafos (ganho médio 1.44 cores, máximo 11).
- Oráculo melhor que a melhor das 4 heurísticas em 9% dos grafos
  (ganho médio -0.93 cores); pior que ela em 33%.
