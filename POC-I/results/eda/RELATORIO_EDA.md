# Análise exploratória da base

Gerado automaticamente por `scripts/eda.py` a partir de `data/` (camadas A, S e B). O conjunto DIMACS não é usado aqui: é reservado ao teste final.

## 1. Visão geral da base

Camada B: 813 grafos (subconjunto de A com n ≤ 500 e réplicas 0–2), 858.790 linhas de candidatos.

| camada | modelo | grafos | n_min | n_max | arestas_max | dens_min | dens_max |
|---|---|---|---|---|---|---|---|
| A | Barabási–Albert | 855 | 50 | 1000 | 210.000 | 0,004 | 0,429 |
| A | Erdős–Rényi | 1900 | 50 | 1000 | 474.750 | 0,038 | 0,954 |
| S | Barabási–Albert | 36 | 2000 | 10.000 | 199.600 | 0,000 | 0,020 |
| S | Erdős–Rényi | 27 | 2000 | 10.000 | 10.001.300 | 0,050 | 0,200 |

Medianas dos atributos globais na camada A:

| atributo | Barabási–Albert | Erdős–Rényi |
|---|---|---|
| nº de vértices (`n`) | 550 | 525 |
| nº de arestas (`m`) | 4.725 | 49.403 |
| densidade (`density`) | 0,0622 | 0,5 |
| grau mínimo (`deg_min`) | 8 | 171 |
| grau máximo (`deg_max`) | 120 | 232 |
| grau médio (`deg_mean`) | 18 | 200 |
| desvio do grau (`deg_std`) | 14,2 | 8,92 |
| coef. de variação do grau (`deg_cv`) | 0,742 | 0,0479 |
| grau máximo / grau médio (`deg_max_ratio`) | 4,9 | 1,14 |
| agrupamento médio (`avg_clustering`) | 0,149 | 0,5 |
| agrupamento / densidade (`clust_ratio`) | 2,29 | 1 |
| degenerescência (`degeneracy`) | 10 | 177 |
| degenerescência / grau máximo (`degen_ratio`) | 0,106 | 0,777 |

![Figura 1 — Distribuição dos atributos globais (camada A), por modelo gerador.](figs/1_distribuicoes.png)

*Figura 1 — Distribuição dos atributos globais (camada A), por modelo gerador.*

Os grafos ER cobrem densidades de 0,05 a 0,95 com grau quase homogêneo (coeficiente de variação baixo) e agrupamento ≈ densidade (razão ≈ 1). Os BA são esparsos na maior parte da grade, com grau heterogêneo e agrupamento acima do esperado para a densidade. As duas famílias ocupam regiões quase disjuntas do espaço de atributos, o que limita o que se pode concluir sobre grafos "intermediários" (ex.: densos e heterogêneos).

## 2. Desempenho das heurísticas (camada A, rótulo por grafo)

| heurística | melhor c/ empate (geral) | melhor sozinha (geral) | melhor c/ empate (ER) | melhor sozinha (ER) | melhor c/ empate (BA) | melhor sozinha (BA) | cores acima da melhor (média) | % acima da melhor (média) | % acima da melhor (máx.) |
|---|---|---|---|---|---|---|---|---|---|
| Aleatória | 0% | 0% | 0% | 0% | 0% | 0% | 12,48 | 36,9% | 166,7% |
| Welsh-Powell | 3% | 0% | 1% | 0% | 8% | 0% | 7,78 | 16,4% | 66,7% |
| DSATUR | 38% | 23% | 11% | 1% | 100% | 73% | 3,95 | 5,0% | 25,0% |
| RLF | 77% | 62% | 99% | 89% | 27% | 0% | 0,34 | 3,1% | 33,3% |

Categoria vencedora por grafo. "WP/aleatória" = Welsh-Powell ou ordem aleatória estritamente melhor que DSATUR e RLF — **não ocorre em nenhum grafo**: a disputa é sempre entre DSATUR e RLF.

| vencedora (categoria) | Barabási–Albert | Erdős–Rényi | geral |
|---|---|---|---|
| DSATUR | 73% | 1% | 23% |
| RLF | 0% | 89% | 62% |
| empate DSATUR = RLF | 27% | 9% | 15% |

![Figura 2 — Heurística vencedora e diferença relativa DSATUR − RLF no espaço de parâmetros da camada A.](figs/2_mapas_vencedora.png)

*Figura 2 — Heurística vencedora e diferença relativa DSATUR − RLF no espaço de parâmetros da camada A.*

No ER, o RLF é estritamente melhor que o DSATUR na maioria das réplicas para p em 0,10–0,95; os empates concentram-se em n pequeno ou p pequeno, onde as colorações usam poucas cores. No BA com m ≤ 8, DSATUR e RLF empatam em 41% dos grafos; com m > 8, o RLF vence sozinho em 0% e o DSATUR em 87%.

![Figura 3 — Excesso de cores em relação à melhor heurística (caixas sem pontos extremos) e tempo mediano por n.](figs/2_excesso_e_tempo.png)

*Figura 3 — Excesso de cores em relação à melhor heurística (caixas sem pontos extremos) e tempo mediano por n.*

## 3. Atributos do grafo e heurística vencedora

Informação mútua (IM) entre cada atributo, discretizado em 10 quantis, e a categoria vencedora, **normalizada pela entropia da categoria** (0 = nenhuma informação; 1 = determina a vencedora). A última coluna é a correlação de Spearman com a diferença relativa DSATUR − RLF (positiva: RLF melhor).

| atributo | IM geral | IM ER | IM BA | ρ Spearman c/ (D−R)/melhor |
|---|---|---|---|---|
| coef. de variação do grau (`deg_cv`) | 0,59 | 0,39 | 0,10 | -0,70 |
| agrupamento / densidade (`clust_ratio`) | 0,58 | 0,31 | 0,08 | -0,62 |
| degenerescência / grau máximo (`degen_ratio`) | 0,58 | 0,36 | 0,07 | 0,69 |
| grau máximo / grau médio (`deg_max_ratio`) | 0,57 | 0,33 | 0,06 | -0,69 |
| degenerescência (`degeneracy`) | 0,39 | 0,49 | 0,18 | 0,59 |
| grau mínimo (`deg_min`) | 0,39 | 0,47 | 0,19 | 0,59 |
| grau médio (`deg_mean`) | 0,34 | 0,49 | 0,16 | 0,55 |
| densidade (`density`) | 0,32 | 0,16 | 0,09 | 0,53 |
| nº de arestas (`m`) | 0,26 | 0,47 | 0,17 | 0,47 |
| desvio do grau (`deg_std`) | 0,25 | 0,42 | 0,16 | -0,15 |
| agrupamento médio (`avg_clustering`) | 0,22 | 0,17 | 0,08 | 0,45 |
| grau máximo (`deg_max`) | 0,17 | 0,50 | 0,17 | 0,30 |
| nº de vértices (`n`) | 0,05 | 0,26 | 0,08 | 0,07 |

![Figura 4 — Fração de grafos em cada categoria vencedora, por decil de quatro atributos globais.](figs/3_vencedora_por_decil.png)

*Figura 4 — Fração de grafos em cada categoria vencedora, por decil de quatro atributos globais.*

![Figura 5 — Correlação de Spearman entre os atributos globais.](figs/3_correlacao_atributos.png)

*Figura 5 — Correlação de Spearman entre os atributos globais.*

Blocos de atributos quase redundantes (ligados por |ρ| ≥ 0,95): {`m`, `deg_min`, `deg_mean`, `degeneracy`}; {`density`, `avg_clustering`}; {`deg_cv`, `deg_max_ratio`, `degen_ratio`}. Dentro de cada bloco, um atributo basta. Parte da redundância vem da própria grade: no ER, agrupamento ≈ densidade por construção; e `deg_cv`, `deg_max_ratio` e `degen_ratio` separam principalmente ER de BA.

## 4. Escala (camada S)

Médias por configuração; as linhas com n = 500 e 1.000 vêm da camada A (5 réplicas), as demais da camada S (3 réplicas).

| modelo | parâmetro | n | grafos | cores Aleatória | cores Welsh-Powell | cores DSATUR | cores RLF | RLF < DSATUR | DSATUR < RLF | (D−R)/melhor |
|---|---|---|---|---|---|---|---|---|---|---|
| Barabási–Albert | 2,00 | 500 | 5 | 6,4 | 4,0 | 3,2 | 4,0 | 0% | 80% | -26,7% |
| Barabási–Albert | 2,00 | 1000 | 5 | 6,4 | 4,0 | 3,2 | 4,0 | 0% | 80% | -26,7% |
| Barabási–Albert | 2,00 | 2000 | 3 | 7,0 | 4,0 | 3,0 | 4,0 | 0% | 100% | -33,3% |
| Barabási–Albert | 2,00 | 5000 | 3 | 7,0 | 4,7 | 3,0 | 4,0 | 0% | 100% | -33,3% |
| Barabási–Albert | 2,00 | 10.000 | 3 | 8,0 | 4,0 | 3,0 | 4,0 | 0% | 100% | -33,3% |
| Barabási–Albert | 5,00 | 500 | 5 | 10,2 | 6,6 | 5,6 | 6,0 | 0% | 40% | -8,0% |
| Barabási–Albert | 5,00 | 1000 | 5 | 10,4 | 6,8 | 5,6 | 6,2 | 0% | 60% | -11,3% |
| Barabási–Albert | 5,00 | 2000 | 3 | 10,0 | 7,0 | 5,7 | 6,0 | 0% | 33% | -6,7% |
| Barabási–Albert | 5,00 | 5000 | 3 | 11,7 | 7,0 | 6,0 | 6,0 | 0% | 0% | 0,0% |
| Barabási–Albert | 5,00 | 10.000 | 3 | 13,3 | 7,0 | 5,3 | 6,0 | 0% | 67% | -13,3% |
| Barabási–Albert | 10,00 | 500 | 5 | 14,4 | 9,8 | 8,0 | 9,0 | 0% | 100% | -12,5% |
| Barabási–Albert | 10,00 | 2000 | 3 | 16,0 | 10,0 | 8,0 | 9,0 | 0% | 100% | -12,5% |
| Barabási–Albert | 10,00 | 5000 | 3 | 18,0 | 10,0 | 8,0 | 9,3 | 0% | 100% | -16,7% |
| Barabási–Albert | 10,00 | 10.000 | 3 | 18,3 | 10,3 | 8,3 | 9,3 | 0% | 67% | -12,5% |
| Barabási–Albert | 20,00 | 1000 | 5 | 23,2 | 15,2 | 12,6 | 14,0 | 0% | 100% | -11,3% |
| Barabási–Albert | 20,00 | 2000 | 3 | 23,7 | 15,3 | 12,3 | 14,0 | 0% | 100% | -13,7% |
| Barabási–Albert | 20,00 | 5000 | 3 | 26,0 | 15,7 | 12,0 | 14,0 | 0% | 100% | -16,7% |
| Barabási–Albert | 20,00 | 10.000 | 3 | 26,7 | 15,3 | 12,3 | 14,0 | 0% | 100% | -13,7% |
| Erdős–Rényi | 0,05 | 500 | 5 | 12,8 | 11,2 | 9,8 | 9,0 | 80% | 0% | 8,9% |
| Erdős–Rényi | 0,05 | 1000 | 5 | 19,4 | 17,6 | 15,4 | 14,4 | 60% | 0% | 7,1% |
| Erdős–Rényi | 0,05 | 2000 | 3 | 31,0 | 29,0 | 26,0 | 24,0 | 100% | 0% | 8,3% |
| Erdős–Rényi | 0,05 | 5000 | 3 | 61,3 | 58,3 | 52,7 | 49,0 | 100% | 0% | 7,5% |
| Erdős–Rényi | 0,05 | 10.000 | 3 | 103,7 | 100,3 | 92,7 | 86,0 | 100% | 0% | 7,8% |
| Erdős–Rényi | 0,10 | 500 | 5 | 19,4 | 18,2 | 15,8 | 15,0 | 80% | 0% | 5,3% |
| Erdős–Rényi | 0,10 | 1000 | 5 | 31,8 | 29,4 | 26,4 | 24,4 | 100% | 0% | 8,2% |
| Erdős–Rényi | 0,10 | 2000 | 3 | 52,7 | 50,3 | 45,3 | 42,0 | 100% | 0% | 7,9% |
| Erdős–Rényi | 0,10 | 5000 | 3 | 106,7 | 102,7 | 94,7 | 88,0 | 100% | 0% | 7,6% |
| Erdős–Rényi | 0,10 | 10.000 | 3 | 185,0 | 179,7 | 169,7 | 159,0 | 100% | 0% | 6,7% |
| Erdős–Rényi | 0,20 | 500 | 5 | 32,0 | 30,2 | 27,6 | 25,2 | 100% | 0% | 9,6% |
| Erdős–Rényi | 0,20 | 1000 | 5 | 53,2 | 51,2 | 46,8 | 43,2 | 100% | 0% | 8,3% |
| Erdős–Rényi | 0,20 | 2000 | 3 | 92,0 | 88,7 | 82,3 | 75,7 | 100% | 0% | 8,8% |
| Erdős–Rényi | 0,20 | 5000 | 3 | 191,3 | 187,0 | 176,3 | 165,0 | 100% | 0% | 6,9% |
| Erdős–Rényi | 0,20 | 10.000 | 3 | 342,0 | 335,0 | 320,3 | 301,7 | 100% | 0% | 6,2% |

Na camada S, o RLF é estritamente melhor que o DSATUR em 100% dos grafos ER e em 0% dos BA. O tempo mediano do RLF em n = 10.000 foi de 1,4 s, contra 0,9 s do DSATUR. O padrão da camada A se mantém em escala (mesma heurística à frente em cada modelo).

## 5. Trajetórias por passo (camada A)

![Figura 6 — Cores acumuladas ao longo da construção, relativas ao total da melhor heurística (média das 5 réplicas).](figs/5_trajetorias.png)

*Figura 6 — Cores acumuladas ao longo da construção, relativas ao total da melhor heurística (média das 5 réplicas).*

Fração das cores finais de cada heurística já abertas quando 10%, 25% e 50% dos vértices estão coloridos (média na camada A):

| heurística | BA 10% | BA 25% | BA 50% | ER 10% | ER 25% | ER 50% |
|---|---|---|---|---|---|---|
| Aleatória | 26% | 42% | 65% | 19% | 35% | 60% |
| Welsh-Powell | 88% | 96% | 99% | 24% | 42% | 67% |
| DSATUR | 97% | 99% | 100% | 35% | 55% | 80% |
| RLF | 15% | 23% | 45% | 11% | 24% | 48% |

As heurísticas constroem a coloração de formas estruturalmente diferentes. O DSATUR e o Welsh-Powell abrem a maior parte das cores cedo (no ER, o DSATUR já abriu 80% das suas cores na metade da construção) e depois reutilizam; o RLF fecha uma classe de cor por vez, então abre cores em ritmo quase constante (48% na metade). Consequências: (i) comparar "cores usadas até o passo t" entre heurísticas não indica quem está à frente — a diferença final só se revela no fim; (ii) os atributos por passo (`n_colors_before`, `step_frac`, `avail_color`) têm distribuições diferentes conforme a política que gerou o estado. Um modelo treinado em estados de uma política verá estados diferentes ao seguir a própria política (mudança de distribuição), o que precisa ser considerado na prova de conceito.

## 6. Rótulos por passo (camada B)

As linhas da camada B seguem a **trajetória do oráculo**: os estados vistos são os que o oráculo visita, não os de cada heurística isolada.

Passos: 227.100. Candidatos distintos por passo: 1 = 0,5%, 2 = 2,1%, 3 = 16,2%, 4 = 81,2%. **Passos informativos** (ao menos um candidato com rótulo 0): **58,6%**. Nos demais, todas as sugestões levam ao mesmo valor e o passo não ensina nada ao modelo.

Balanço das classes: rótulo 1 em 65,6% de todas as linhas e em 42,7% das linhas dos passos informativos. Nos passos informativos, a diferença entre o melhor e o pior candidato é de 1,64 cores em média (mediana 1).

Por modelo: 82,2% dos passos são informativos no ER e 6,1% no BA — os passos informativos são 97% ER. No BA quase todas as sugestões levam ao mesmo número de cores (o DSATUR já é, em geral, ótimo ou quase), então o sinal de treino por passo vem essencialmente do ER.

Com que frequência o candidato escolhido pelo oráculo era a sugestão de cada heurística (sugestões coincidentes contam para todas): **todos** — Aleatória 2%, Welsh-Powell 7%, DSATUR 62%, RLF 41%; **informativos** — Aleatória 1%, Welsh-Powell 3%, DSATUR 35%, RLF 66%; **triviais** — Aleatória 4%, Welsh-Powell 12%, DSATUR 100%, RLF 6%. Nos passos triviais o desempate (DSATUR > RLF > WP > aleatória) faz o oráculo seguir o DSATUR; nos informativos ele segue sobretudo o RLF. Os estados da camada B são, portanto, uma mistura dos dois estilos de construção.

Fração dos passos em que o candidato sugerido por cada heurística é ótimo (rótulo 1). Referência: escolher um candidato distinto ao acaso acerta 42,8% dos passos informativos.

| candidato de | acerta (todos os passos) | acerta (informativos) | acerta (inform., ER) | acerta (inform., BA) | única a acertar (inform.) |
|---|---|---|---|---|---|
| Aleatória | 54,2% | 21,9% | 21,1% | 44,9% | 0,3% |
| Welsh-Powell | 56,9% | 26,4% | 25,1% | 66,0% | 0,1% |
| DSATUR | 61,9% | 35,1% | 33,0% | 97,4% | 5,2% |
| RLF | 93,1% | 88,3% | 89,1% | 63,7% | 41,7% |

![Figura 8 — Fração de passos informativos e taxa de acerto das sugestões de cada heurística ao longo da construção.](figs/6_informatividade_acerto.png)

*Figura 8 — Fração de passos informativos e taxa de acerto das sugestões de cada heurística ao longo da construção.*

Regras de um único atributo nos passos informativos: escolher o candidato de maior (`max`) ou menor (`min`) valor; empates entre candidatos contam como sorteio (acerto esperado = fração de rótulos 1 entre os empatados).

| regra | acerto (geral) | acerto (ER) | acerto (BA) |
|---|---|---|---|
| seguir RLF | 88,3% | 89,1% | 63,7% |
| min(avail_color) | 81,9% | 82,7% | 58,0% |
| min(saturation) | 71,2% | 72,1% | 45,7% |
| min(opens_new_color) | 66,3% | 66,4% | 63,8% |
| min(colored_nb) | 57,5% | 57,8% | 48,6% |
| min(degree) | 46,6% | 46,4% | 50,5% |
| candidato ao acaso | 42,8% | 42,0% | 66,3% |
| max(uncolored_nb) | 40,8% | 39,9% | 66,3% |
| seguir DSATUR | 35,1% | 33,0% | 97,4% |
| min(uncolored_nb) | 35,1% | 34,4% | 53,7% |
| max(saturation) | 31,7% | 29,5% | 95,6% |
| max(avail_color) | 27,8% | 25,9% | 85,7% |
| max(colored_nb) | 27,0% | 25,3% | 79,2% |
| max(degree) | 26,6% | 25,3% | 66,2% |
| seguir Welsh-Powell | 26,4% | 25,1% | 66,0% |
| max(opens_new_color) | 25,3% | 23,7% | 71,9% |
| seguir Aleatória | 21,9% | 21,1% | 44,9% |

Dentro de cada passo informativo: média do atributo nos candidatos ótimos menos a média nos não ótimos.

| atributo | diferença média | rótulo 1 maior | iguais | rótulo 1 menor |
|---|---|---|---|---|
| degree | -2,54 | 36% | 2% | 63% |
| saturation | -0,50 | 21% | 7% | 72% |
| uncolored_nb | 0,69 | 54% | 4% | 43% |
| colored_nb | -3,23 | 28% | 3% | 69% |
| avail_color | -0,84 | 18% | 4% | 78% |
| opens_new_color | -0,41 | 6% | 28% | 67% |

Ganho do oráculo sobre a melhor das 4 heurísticas:

| modelo | grafos | melhora | ganho médio (cores) | ganho máx. (cores) | ganho médio (%) | tempo médio (s) |
|---|---|---|---|---|---|---|
| Barabási–Albert | 243 | 13% | 0,13 | 1 | 1,5% | 11,40 |
| Erdős–Rényi | 570 | 73% | 1,48 | 9 | 3,5% | 14,50 |

![Figura 9 — Ganho relativo do oráculo sobre a melhor heurística, por região da camada B (média das réplicas).](figs/6_ganho_oraculo.png)

*Figura 9 — Ganho relativo do oráculo sobre a melhor heurística, por região da camada B (média das réplicas).*

## 7. Conclusões para o modelo (sugestões, a discutir)

1. **A disputa é entre DSATUR e RLF, e a família do grafo quase a decide.** Welsh-Powell e ordem aleatória nunca são estritamente as melhores. No ER o RLF vence (ou empata) quase sempre; no BA, o DSATUR. Os atributos globais mais informativos (um por bloco redundante) são `deg_cv`, `clust_ratio`, `degeneracy`, mas eles separam sobretudo ER de BA: dentro de cada família, os mais informativos são `deg_max` (ER) e `deg_min` (BA). Um classificador por grafo (opção 1) tende a aprender a família geradora; ele deve ser avaliado também separadamente por modelo, e o teste no DIMACS dirá se generaliza.
2. **Atributos redundantes.** Blocos com |ρ| ≥ 0,95: {`m`, `deg_min`, `deg_mean`, `degeneracy`}; {`density`, `avg_clustering`}; {`deg_cv`, `deg_max_ratio`, `degen_ratio`}. Basta um atributo por bloco.
3. **Rótulo por passo (opção 3b): treinar só nos passos informativos.** Apenas 58,6% dos passos da camada B são informativos (82,2% no ER, 6,1% no BA). Treinar em todos inflaria a acurácia com exemplos triviais; sugere-se treinar e avaliar a classificação apenas nos passos informativos e reportar à parte a fração de triviais. O sinal de treino por passo vem quase todo do ER.
4. **Linha de base a superar.** Nos passos informativos, seguir o RLF acerta 88,3% (no BA, onde há poucos passos informativos, a melhor sugestão é a do DSATUR, com 97,4%); a melhor regra de um atributo (`min(avail_color)`) acerta 81,9%; ao acaso, 42,8%. A regra `min(avail_color)` é essencialmente o princípio do RLF (completar a classe de cor atual antes de abrir outra), o que explica seu bom desempenho. O modelo precisa superar o candidato do RLF — e, sobretudo, usar menos cores ao **seguir a própria política** de ponta a ponta, que é a métrica do objetivo geral.
5. **Mudança de distribuição.** Os estados da camada B vêm da trajetória do oráculo (uma mistura de passos no estilo DSATUR e no estilo RLF), e cada política produz estados com distribuições diferentes (seção 5). Ao seguir a própria política, o modelo verá estados que não estavam no treino. Para a prova de conceito isso é aceitável, mas deve ser medido (acerto por passo versus cores finais) e registrado como limitação a tratar no POC II (ex.: gerar rótulos também nos estados visitados pelo próprio modelo).
6. **Formulação sugerida para a prova de conceito:** classificação binária de candidatos (rótulo 3b) nos passos informativos, com atributos do candidato (tabela da seção 6), atributos relativos ao passo (ex.: `avail_color` comparado ao nº de cores já usadas; posição de cada atributo entre os candidatos do passo) e atributos globais do grafo (um por bloco); escolha do candidato de maior probabilidade; divisão treino/validação por grafo, estratificada por modelo e n; avaliação por acerto nos passos informativos e por nº de cores da coloração completa obtida com a política aprendida, comparado ao DSATUR, ao RLF e ao oráculo.
