# Proposta de Trabalho — POC II (rascunho)

**Aprendizado de Máquina para Seleção de Escolhas Gulosas em Algoritmos de Coloração de Grafos — parte II**

Aluno: Lucas Affonso Pires · Orientador: Prof. Marcio Costa Santos · DCC/UFMG
Tipo de pesquisa: científica

> Rascunho elaborado a partir dos resultados do POC I (`docs/relatorio/artigo.tex`), para
> revisão do aluno e do orientador antes de ser formatado no modelo da disciplina.

## 1. Introdução

No POC I foram implementadas as heurísticas gulosas de referência (ordem aleatória,
Welsh-Powell, DSATUR e RLF) num arcabouço único, construída uma base reprodutível com 2.818
grafos sintéticos e 858.790 candidatos rotulados por *rollout*, feita a análise exploratória e
uma prova de conceito de política aprendida. O principal resultado foi que um classificador
com 97,2% de acerto por passo produz colorações **iguais** às da melhor heurística em 96% dos
grafos de validação e em 90% das 127 instâncias DIMACS: ele aprendeu a seguir o RLF ou o DSATUR
conforme a família do grafo, e seus poucos erros por passo (~5 por grafo) consomem o ganho
possível, de pouco mais de uma cor. O oráculo usado como rótulo, porém, supera a melhor
heurística em 55% dos grafos sintéticos rotulados e em 24% das instâncias DIMACS com até 500
vértices: há espaço para aprender.

**Objetivo geral (mantido):** desenvolver e avaliar um modelo que, a cada passo da construção
gulosa, escolha o próximo vértice de modo a usar número de cores igual ou menor que as
heurísticas clássicas, comparando-o às heurísticas de referência em instâncias de *benchmark*.

**Objetivos específicos do POC II:**
1. Treinar o modelo com foco nos passos decisivos: ponderar cada exemplo pela diferença de
   valor entre os candidatos e avaliar uma formulação de ordenação (*learning to rank*).
2. Reduzir a mudança de distribuição com DAgger: rotular por *rollout* os estados visitados
   pela própria política e retreinar iterativamente.
3. Selecionar e ajustar modelos pelo número final de cores de ponta a ponta (e não pela
   acurácia por passo) e avaliar ajuste fino por aprendizado por reforço.
4. Ampliar os dados de treino com famílias estruturadas (por exemplo, grafos geométricos e
   quase k-coloríveis), gerados de forma independente das instâncias de teste, para cobrir a
   região do espaço de atributos ocupada pelo DIMACS.
5. Calcular as sugestões das heurísticas de forma incremental, aproximando o tempo da política
   ao das heurísticas.
6. Se os resultados anteriores justificarem, avaliar um modelo que pontue candidatos além das
   quatro sugestões (por exemplo, com redes neurais em grafos).
7. Consolidar os resultados num artigo científico no modelo IEEE.

## 2. Referencial teórico

O referencial do POC I (artigo, Seção II) é mantido e aprofundado em três frentes:
aprendizado por imitação e mudança de distribuição (DAgger; Ross, Gordon e Bagnell, 2011),
aprendizado de ordenação e aprendizado por reforço para otimização combinatória (Khalil et
al., 2017; Bengio, Lodi e Prouvost, 2021) e redes neurais em grafos para coloração (Lemos et
al., 2019; Watkins, Montana e Branke, 2023; Langedal e Manne, 2024).

## 3. Metodologia

1. **Linha de base:** a política do POC I (gradient boosting) e as quatro heurísticas, com as
   mesmas divisões por grafo e as mesmas métricas (acerto nos passos informativos, número de
   cores de ponta a ponta, generalização na réplica 3 da camada A).
2. **Treino focado:** pesos por diferença de `rollout_colors` entre candidatos; perda de
   ordenação par a par; comparação com a linha de base.
3. **DAgger:** a cada iteração, a política atual colore os grafos de treino; os estados
   visitados são rotulados com o mesmo *rollout* (DSATUR e RLF como políticas-base), somados à
   base e usados no retreino. Custo estimado a partir do piloto da camada B (~3 h de CPU por
   rotulagem completa).
4. **Seleção por cores finais:** validação cruzada por grafo, escolhendo hiperparâmetros pelo
   número médio de cores da política.
5. **Novos dados e eficiência:** novas camadas sintéticas estruturadas; candidatos
   incrementais.
6. **Avaliação final:** o conjunto DIMACS continua sendo apenas de teste; o modelo final é
   congelado antes de ser aplicado, uma única vez. Como o DIMACS já foi usado no POC I,
   considerar também um segundo conjunto de teste ainda não utilizado.

## 4. Resultados esperados

Uma política aprendida que use **menos** cores que a melhor heurística numa fração
significativa das instâncias, sem perdas sistemáticas por família, com custo de tempo
compatível com uso prático; e uma análise de quais fatores (ponderação, DAgger, dados,
modelo) explicam o ganho.

## 5. Cronograma (16 semanas)

| Semanas | Atividade |
|---|---|
| 1–2 | Revisão bibliográfica (imitação, ordenação, GNN) e plano experimental detalhado |
| 3–5 | Treino focado nos passos decisivos e seleção por cores finais |
| 6–8 | DAgger: rotulagem dos estados da política e retreino iterativo |
| 9–10 | Novas famílias de grafos de treino e cálculo incremental dos candidatos |
| 11–12 | (Condicional) modelo com candidatos além das sugestões das heurísticas |
| 13–14 | Avaliação final: validação cruzada e teste único no DIMACS |
| 15–16 | Artigo final, apresentação e documentação |

## 6. Referências

As referências completas estão em `docs/relatorio/referencias.bib`.
