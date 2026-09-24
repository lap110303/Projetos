# CLAUDE.md — POC I: aprendizado de máquina para escolhas gulosas em coloração de grafos

Contexto persistente para o Claude Code. Leia antes de qualquer alteração.

## Projeto

- Trabalho de Projeto Orientado em Computação I (POC I), DCC/UFMG.
- Aluno: Lucas Affonso Pires. Orientador: Prof. Marcio Costa Santos.
- Duração: 2 semestres (POC I e POC II). A proposta está em `LucasAffonsoPires.pdf`
  (fora desta pasta).
- **Objetivo geral:** treinar um modelo que, a cada passo da construção gulosa de uma
  coloração de vértices, escolha o próximo vértice de modo a usar número de cores igual
  ou menor que as heurísticas clássicas (Welsh-Powell, DSATUR, RLF).
- **Objetivos do POC I:** implementar as heurísticas, construir a base de dados, fazer
  análise exploratória e uma prova de conceito do modelo preditivo, e consolidar tudo
  num relatório técnico.
- Idioma: toda documentação, comentários, docstrings e mensagens em **português**.

## Estado atual (setembro/2026)

Concluído:
- Heurísticas implementadas e testadas: ordem aleatória, Welsh-Powell, DSATUR, RLF.
- Geradores ER e BA, definição das camadas da base, rollouts, extração de atributos,
  gravação de grafos, leitura DIMACS.
- 33 testes passando (`python -m pytest`).
- Piloto de custo executado (`results/pilot/RELATORIO_PILOTO.md`).

Próximos passos, nesta ordem:
1. Script `scripts/build_dataset.py` para gerar a base completa em `data/`, reutilizando
   `poc/pipeline.py`. Requisitos: paralelismo por grafo (`multiprocessing`), retomada
   após interrupção (pular grafos já processados), gravação em Parquet (zstd) em lotes,
   validação de toda coloração, log de progresso.
2. Baixar e processar as instâncias DIMACS de coloração (confirmar quantas são; estimativa
   de 120 a 140) com as 4 heurísticas e, nas pequenas, o oráculo por rollout.
3. Análise exploratória (objetivo 5 da proposta).
4. Prova de conceito do modelo (objetivo 6).

## Decisões tomadas (não alterar sem discutir com o aluno)

### Base de dados — ver `docs/ESPECIFICACAO_BASE.md`
- **Camada A (2.755 grafos):** ER com n = 50..1000 (passo 50), p = 0,05..0,95 (passo
  0,05), 5 réplicas; BA com o mesmo n, m ∈ {2,3,5,8} ∪ {2,5,10,20,30% de n}, 5 réplicas.
  Uso: heurística vencedora por grafo ("opção 1") e trajetórias por passo.
- **Camada B (813 grafos):** subconjunto de A com n ≤ 500 e réplicas 0–2. Uso: rótulo por
  passo via rollout ("opção 3b").
- **Camada S (63 grafos):** n ∈ {2000, 5000, 10000}, ER p ∈ {0,05; 0,1; 0,2}, BA
  m ∈ {2,5,10,20}, 3 réplicas. Só avaliação de escala, sem rollout.
- **DIMACS:** conjunto de TESTE final, usado uma única vez. Nunca usar para treino,
  validação ou ajuste de hiperparâmetros.
- Divisão treino/validação sempre **por grafo**, nunca por vértice (evita vazamento).
- A grade original do aluno (n até 10.000 com todos os p, BA com m até 30% de n para
  todo n) foi descartada por ser inviável (~10¹² arestas no BA).

### Rótulos
- **Opção 1 (camada A):** rótulo por grafo = heurística com menos cores.
- **Opção 3b (camada B):** a cada passo, cada heurística sugere um candidato (até 4
  distintos). Cada candidato é colorido e a coloração é completada **com DSATUR e com
  RLF**; o valor do candidato é o menor dos dois. Rótulo = 1 para os que atingem o
  mínimo (empates todos = 1). A trajetória segue o melhor candidato (desempate
  dsatur > rlf > welsh_powell > random).
- Versão 1 do rollout completava só com DSATUR e o oráculo ficava pior que a melhor
  heurística em 33% dos grafos. Resultados preservados em
  `results/pilot/v1_rollout_somente_dsatur/` para o relatório. **Não apagar.**
- Garantia da versão 2 (melhoria sequencial, Bertsekas): o oráculo nunca é pior que
  min(DSATUR, RLF). Há teste para isso.

### Implementação
- Grafos como **matriz de adjacência densa booleana (numpy)**, não networkx. networkx só no
  gerador BA e como referência nos testes.
- Toda heurística é uma *regra de escolha* sobre `ColoringState`; o vértice escolhido
  recebe sempre a menor cor disponível. Isso vale também para o RLF (cada classe é
  fechada de forma maximal antes da próxima).
- O RLF tem três implementações que DEVEM continuar equivalentes (testado):
  `rlf` (do zero, rápida), `rlf_choice` (regra sem memória, qualquer estado) e
  `rlf_complete` (incremental, a partir de qualquer estado; usada nos rollouts).
- Desempates sempre pelo menor índice de vértice. DSATUR desempata pelo maior grau no
  subgrafo não colorido (Brélaz, 1979).
- Ids estáveis de grafos (`er_n500_p0.35_s2`, `ba_n1000_m50_s4`); semente derivada do id
  por SHA-256 (`poc.graphs.stable_seed`). Ordem aleatória usa
  `stable_seed(id, "random-order")`.
- Versões fixadas em `requirements.txt` (o gerador do networkx pode mudar entre versões).

## Estrutura

```
poc/          pacote: graphs, grids, state, heuristics, rollout, features,
              pipeline, storage, io_dimacs, exact
tests/        pytest (conftest com grafos de teste)
scripts/      pilot.py (piloto de custo)
results/      saídas de experimentos (pilot/ e pilot/v1_rollout_somente_dsatur/)
docs/         ESPECIFICACAO_BASE.md
data/         destino da base completa (vazia)
```

## Comandos

```bash
pip install -r requirements.txt
python -m pytest                     # rodar sempre antes de concluir uma alteração
python scripts/pilot.py --per-stratum-a 2 --per-stratum-b 2 --per-stratum-s 3
```

## Números de referência do piloto (1 núcleo, 3 GB de RAM)

- Tempo total estimado da base: ~2h15 de CPU (A ~6 min, S ~8 min, B ~2 h).
- Disco: ~160 MB (grafos .npz + tabelas Parquet), mais ~100 MB do DIMACS.
- Pior caso de memória: ~560 MB (n = 10.000, p = 0,2).
- Prévia: DSATUR e RLF vencem cada um sozinhos em ~42% dos grafos; RLF perde por pouco
  quando perde. O oráculo v2 supera a melhor heurística em 47% dos grafos da amostra B e
  empata no restante.

## Regras de trabalho

- Rodar `python -m pytest` antes de dar qualquer tarefa por concluída.
- Toda coloração produzida deve ser validada (`is_valid_coloring`).
- Não mudar grades, sementes, desempates ou definição de rótulos sem confirmação do aluno:
  isso invalida a reprodutibilidade da base.
- Resultados de experimentos antigos são preservados em subpastas nomeadas, não
  sobrescritos, quando a mudança for metodológica.
- Atualizar este arquivo, o README e `docs/ESPECIFICACAO_BASE.md` quando uma decisão mudar.
