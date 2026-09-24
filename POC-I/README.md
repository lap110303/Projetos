# POC I — Aprendizado de máquina para seleção de escolhas gulosas em coloração de grafos

Aluno: Lucas Affonso Pires · Orientador: Prof. Marcio Costa Santos · DCC/UFMG

## Estrutura

```
POC-I/
├── poc/                    # pacote principal
│   ├── graphs.py           # geradores ER e BA, ids estáveis e sementes
│   ├── grids.py            # definição das camadas A, B e S da base
│   ├── state.py            # estado parcial da coloração gulosa
│   ├── heuristics.py       # aleatória, Welsh-Powell, DSATUR e RLF
│   ├── rollout.py          # rótulos por passo via rollout (camada B)
│   ├── features.py         # atributos por passo e atributos globais do grafo
│   ├── pipeline.py         # processamento completo de um grafo
│   ├── storage.py          # gravação compacta de grafos (.npz)
│   ├── io_dimacs.py        # leitura/escrita do formato DIMACS (.col)
│   └── exact.py            # número cromático exato (só para testes)
├── tests/                  # testes automatizados (pytest)
├── scripts/
│   └── pilot.py            # piloto de tempo, memória e disco
├── results/pilot/          # saídas do piloto (CSV, JSON, relatório)
├── docs/
│   └── ESPECIFICACAO_BASE.md
├── data/                   # (vazia) destino da base completa
├── CLAUDE.md               # contexto do projeto para o Claude Code
└── requirements.txt
```

## Como usar

```bash
cd POC-I
pip install -r requirements.txt
python -m pytest              # 33 testes
python scripts/pilot.py --per-stratum-a 2 --per-stratum-b 2 --per-stratum-s 3
```

O piloto grava em `results/pilot/` o relatório `RELATORIO_PILOTO.md`, o `summary.json`
com as projeções e um CSV por camada com as medições de cada grafo.

## Decisões de implementação

**Representação dos grafos.** Matriz de adjacência densa booleana (numpy). Para os
tamanhos do projeto (n ≤ 10.000) ela cabe em memória (até 100 MB) e permite que cada
passo guloso seja uma operação vetorizada, o que é muito mais rápido que networkx.
O networkx é usado apenas no gerador BA e como referência nos testes.

**Um único esquema para todas as heurísticas.** Toda heurística é uma *regra de escolha*
aplicada a um estado parcial (`ColoringState`); o vértice escolhido sempre recebe a menor
cor disponível. Isso permite misturar regras ao longo da construção (necessário nos
rollouts) e extrair os mesmos atributos por passo para todas as heurísticas.

**RLF no mesmo esquema.** O RLF constrói uma classe de cor por vez, mas como cada classe
é fechada de forma maximal antes da próxima, a cor que ele atribui coincide com a menor
cor disponível. Há duas implementações: uma incremental (rápida, para execuções
completas) e uma regra "sem memória" (`rlf_choice`), aplicável a qualquer estado parcial.
Um teste garante que ambas produzem exatamente a mesma ordem e as mesmas cores.

**Desempates.** Sempre pelo menor índice de vértice, o que torna tudo determinístico.
O DSATUR desempata pelo maior grau no subgrafo não colorido (Brélaz, 1979).

**Rollouts (camada B), versão 2.** Cada candidato é testado completando a coloração
duas vezes, com o DSATUR e com o RLF, e vale o menor dos dois resultados. Como os
candidatos do DSATUR e do RLF estão sempre entre os testados e ambas as regras são
determinísticas, o número de cores do melhor candidato nunca aumenta ao longo da
trajetória, e a coloração "oráculo" nunca é pior que min(DSATUR, RLF). A mesma
propriedade permite herdar dois dos valores do passo anterior, poupando duas conclusões
por passo. A versão 1 completava só com o DSATUR e ficava pior que a melhor heurística em
33% dos grafos do piloto; seus resultados estão em `results/pilot/v1_rollout_somente_dsatur/`.

## O que os testes verificam

- Toda heurística produz coloração própria, respeita a cota Δ+1 e nunca usa menos
  cores que o número cromático exato (grafos pequenos, Petersen, Grötzsch).
- Grafos sem arestas usam 1 cor; grafos completos usam n cores.
- DSATUR é exato em grafos bipartidos e ciclos (resultado de Brélaz).
- Welsh-Powell coincide com `networkx.greedy_color(strategy="largest_first")` e respeita a
  cota de Welsh-Powell.
- As duas implementações do RLF são idênticas, e as conclusões de DSATUR e RLF a partir
  de estados parciais arbitrários coincidem com a aplicação passo a passo das regras.
- Geradores: reprodutibilidade, número de arestas do BA = m(n−m), densidade do ER ≈ p,
  tamanhos das camadas (2.755 / 813 / 63).
- Rollout: coloração oráculo válida e nunca pior que min(DSATUR, RLF); valor do candidato
  escolhido não cresce ao longo da trajetória; rótulos consistentes.
- Atributos globais conferem com networkx; gravação e leitura de grafos e DIMACS.
