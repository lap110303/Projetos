# Especificação da base de dados

Definição implementada em `poc/grids.py` e verificada em `tests/test_graphs.py`.

## Camadas

| Camada | Uso | Grafos |
|---|---|---|
| A – Principal | Heurística vencedora por grafo (opção 1), trajetórias, análise exploratória | 2.755 |
| B – Rollout | Rótulo por passo (opção 3b), prova de conceito do modelo | 813 (subconjunto de A) |
| S – Escala | Avaliação em grafos grandes, sem rótulo por passo | 63 |
| DIMACS | Teste final, usado uma única vez | ~120–140 (a confirmar) |

### Camada A
- **Erdős–Rényi:** n = 50, 100, …, 1000; p = 0,05, 0,10, …, 0,95; 5 réplicas → 1.900 grafos.
- **Barabási–Albert:** n = 50, 100, …, 1000; m ∈ {2, 3, 5, 8} ∪ {2%, 5%, 10%, 20%, 30% de n}
  (sem duplicatas, 2 ≤ m < n); 5 réplicas → 855 grafos.

### Camada B
Grafos da camada A com n ≤ 500 e réplicas 0, 1 e 2 (570 ER + 243 BA).

A cada passo, as quatro heurísticas indicam um candidato. Cada candidato distinto é
colorido e a coloração é completada duas vezes, com o DSATUR e com o RLF; o valor do
candidato é o menor dos dois resultados. Rótulo = 1 para os candidatos que atingem o
menor valor. A trajetória segue o melhor candidato, o que produz uma coloração
"oráculo" que nunca é pior que min(DSATUR, RLF).

### Camada S
n ∈ {2000, 5000, 10000}; ER com p ∈ {0,05, 0,1, 0,2}; BA com m ∈ {2, 5, 10, 20};
3 réplicas → 27 ER + 36 BA.

## Identificadores e sementes

Cada grafo tem um id estável, por exemplo `er_n500_p0.35_s2` ou `ba_n1000_m50_s4`.
A semente do gerador é derivada do id por SHA-256 (`poc.graphs.stable_seed`), então o
mesmo id sempre gera o mesmo grafo e ids diferentes usam fluxos aleatórios independentes.
A ordem aleatória da heurística "guloso simples" usa a semente `stable_seed(id, "random-order")`.

## Tabelas geradas

| Arquivo | Granularidade | Conteúdo |
|---|---|---|
| `graphs.parquet` | 1 linha por grafo | id, modelo, n, parâmetro, réplica, m, densidade, graus (mín/máx/média/desvio), agrupamento médio, degenerescência |
| `runs.parquet` | 1 linha por (grafo, heurística) | número de cores, tempo |
| `steps.parquet` | 1 linha por (grafo, heurística, passo) | vértice escolhido, grau, saturação, vizinhos coloridos/não coloridos, cor recebida, abre cor nova?, cores já usadas, passo/n |
| `rollouts.parquet` | 1 linha por (grafo, passo, candidato) | heurísticas que sugeriram o candidato, atributos do candidato, cores ao completar com DSATUR e com RLF, valor (mínimo dos dois), rótulo, escolhido? |

Os grafos são salvos em `.npz` comprimido, no formato mais compacto entre lista de
arestas (uint16) e triângulo superior da matriz em bits (`poc/storage.py`).
