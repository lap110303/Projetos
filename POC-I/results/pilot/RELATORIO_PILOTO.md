# Relatório do piloto

Gerado automaticamente por `scripts/pilot.py`.

## Ambiente

Python 3.12.3, numpy 2.4.4, pandas 3.0.2, 1 núcleo(s).
Duração do piloto: 9.7 min. Pico de memória (RSS): 558 MB.

## Projeção para a base completa (1 núcleo)

| Camada | Grafos | Medidos | Tempo de CPU estimado | Arquivos de grafos | Linhas por passo | Parquet |
|---|---|---|---|---|---|---|
| A | 2,755 | 82 | 5.9 ± 0.2 min | 43 MB | 5,854,000 | 41 MB |
| S | 63 | 19 | 7.8 ± 0.1 min | 60 MB | 1,428,000 | 10 MB |
| B | 813 | 43 | 120.4 ± 2.8 min | (reusa A) | 858,526 | 7 MB |
| **Total** | | | **134 min** | | | **161 MB** (grafos + tabelas) |

Os valores "±" são erros-padrão da estimativa estratificada. Bytes por linha em Parquet (zstd):
7.1 (passos) e 8.2 (rollouts).

## Piores casos — camadas A e S

| Grafo | Arestas | Tempo total (s) | Arquivo (MB) | Cores (Aleat./WP/DSATUR/RLF) |
|---|---|---|---|---|
| ba_n1000_m300_s0 | 210,000 | 0.64 | 0.05 | 130/98/94/97 |
| er_n1000_p0.95_s0 | 474,645 | 0.34 | 0.02 | 383/386/372/334 |
| ba_n10000_m20_s0 | 199,600 | 6.34 | 0.49 | 26/15/13/14 |
| er_n10000_p0.20_s0 | 9,997,369 | 34.02 | 4.85 | 342/336/322/302 |

## Piores casos — camada B (rollout)

| Grafo | Tempo (s) | Conclusões | Oráculo | DSATUR | RLF | Melhor heurística |
|---|---|---|---|---|---|---|
| ba_n500_m150_s0 | 26.7 | 2,856 | 55 | 55 | 56 | 55 |
| er_n500_p0.95_s0 | 34.1 | 2,850 | 179 | 204 | 183 | 183 |
| er_n500_p0.50_s0 | 29.8 | 2,906 | 58 | 66 | 60 | 60 |

## Prévia: desempenho das heurísticas na amostra da camada A (80 grafos)

Amostra pequena, apenas indicativa; a análise exploratória usará a base completa.

| Heurística | Melhor (com empate) | Melhor sozinha | Cores acima da melhor (média) |
|---|---|---|---|
| Aleatória | 0% | 0% | 12.74 |
| Welsh-Powell | 5% | 0% | 6.92 |
| DSATUR | 57% | 42% | 3.20 |
| RLF | 56% | 42% | 0.62 |

## Prévia: coloração oráculo (rollout) na amostra da camada B (43 grafos)

Rollouts completados com DSATUR e com RLF (versão 2). A versão 1, que completava só
com o DSATUR, está em `v1_rollout_somente_dsatur/`.

- Oráculo melhor que o DSATUR em 56% dos grafos (ganho médio 3.47 cores, máximo 25).
- Oráculo melhor que o RLF em 79% dos grafos (ganho médio 1.51 cores, máximo 6).
- Oráculo pior que min(DSATUR, RLF) em 0% dos grafos (a garantia teórica é 0%).
- Oráculo melhor que a melhor das 4 heurísticas em 47% dos grafos
  (ganho médio 1.09 cores, máximo 6); igual em 53%;
  pior em 0%.
