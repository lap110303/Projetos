# CLAUDE.md — POC I: aprendizado de máquina para escolhas gulosas em coloração de grafos

Contexto persistente para o Claude Code. Leia antes de qualquer alteração.

## Projeto

- Trabalho de Projeto Orientado em Computação I (POC I), DCC/UFMG.
- Aluno: Lucas Affonso Pires. Orientador: Prof. Marcio Costa Santos.
- Duração: 2 semestres (POC I e POC II). A proposta está em `LucasAffonsoPires.pdf`
  (nesta pasta).
- **Objetivo geral:** treinar um modelo que, a cada passo da construção gulosa de uma
  coloração de vértices, escolha o próximo vértice de modo a usar número de cores igual
  ou menor que as heurísticas clássicas (Welsh-Powell, DSATUR, RLF).
- **Objetivos do POC I:** implementar as heurísticas, construir a base de dados, fazer
  análise exploratória e uma prova de conceito do modelo preditivo, e consolidar tudo
  num relatório técnico.
- Idioma: toda documentação, comentários, docstrings e mensagens em **português**.

## Estado atual (novembro/2026) — POC I concluído

Concluído:
- Heurísticas implementadas e testadas: ordem aleatória, Welsh-Powell, DSATUR, RLF.
- Geradores ER e BA, definição das camadas da base, rollouts, extração de atributos,
  gravação de grafos, leitura DIMACS (texto e binário `.col.b`).
- 43 testes passando (`python -m pytest`).
- Piloto de custo executado (`results/pilot/RELATORIO_PILOTO.md`).
- `scripts/build_dataset.py` implementado e testado (paralelismo por grafo, retomada,
  Parquet zstd em lotes, validação, log). Layout em `docs/ESPECIFICACAO_BASE.md`.
- Base completa gerada em `data/` em 25/09/2026: A 2.755, S 63, B 813 grafos, sem falhas;
  26 min com 8 processos, 136 MB. Integridade conferida (sem duplicatas, contagens de
  linhas corretas). Na camada B o oráculo supera a melhor heurística em 55% dos grafos
  e empata no restante; nunca fica pior que min(DSATUR, RLF).

- Análise exploratória (objetivo 5) feita: `scripts/eda.py` gera
  `results/eda/RELATORIO_EDA.md` (7 seções, 8 figuras, tabelas CSV) em ~10 s. Achados que
  orientam o modelo:
  - A disputa é só entre DSATUR e RLF (WP/aleatória nunca vencem sozinhas). RLF vence no
    ER (89% sozinho), DSATUR no BA (73% sozinho); o padrão se mantém na camada S.
  - Atributos globais que mais informam a vencedora separam sobretudo ER de BA; há três
    blocos redundantes (|ρ| ≥ 0,95).
  - Camada B: só 58,6% dos passos são informativos (82% no ER, 6% no BA). Nos informativos,
    o candidato do RLF acerta 88,3%, `min(avail_color)` 81,9%, ao acaso 42,8%.
  - DSATUR/WP abrem as cores cedo e o RLF em ritmo constante: estados de políticas
    diferentes têm distribuições diferentes (risco de mudança de distribuição).

- Notebook `notebooks/eda.ipynb`: mesma análise, exibida inline (usa `eda.Report(None,
  inline=True)`), com explicações de leitura e células de exploração. Salvo já executado.
- Prova de conceito (objetivo 6) feita com a formulação sugerida, aprovada pelo aluno:
  `poc/model.py` (atributos dos candidatos + `LearnedPolicy`) e `scripts/train_poc.py`
  (relatório em `results/poc_modelo/RELATORIO_MODELO.md`, ~6 min). Resultados:
  - Gradient boosting acerta 97,2% dos passos informativos de validação (RLF: 89,3%;
    `min(avail_color)`: 89,9%); regressão logística 93,1%.
  - De ponta a ponta, porém, a política empata com a melhor heurística em 96% dos grafos
    de validação (2% melhor, 2% pior); nos 551 grafos da réplica 3 (nunca vistos, até
    n = 1.000), 2% melhor e 7% pior. Ela aprendeu a seguir o RLF no ER e o DSATUR no BA.
    Os ~5 erros por grafo (2,8% de ~195 passos informativos) anulam o ganho possível
    (oráculo: ~1,2 cor).

- Teste final no DIMACS feito em 08/11/2026 (`scripts/download_dimacs.py` +
  `scripts/process_dimacs.py`, relatório em `results/dimacs/RELATORIO_DIMACS.md`): 127
  instâncias (79 da página original + 48 do COLOR02/03/04), 69 com χ conhecido. Política
  congelada (modelo sha256 `6bf39632ff60…`): empata com a melhor heurística em 90%, melhor em
  1 instância, pior em 9%; atinge χ em 64% (= RLF). Oráculo (n ≤ 500) melhor que a melhor
  heurística em 24%. **O DIMACS já foi usado; não reprocessar a política nele.**
- Artigo final no modelo IEEE (exigido para trabalho científico) em `docs/relatorio/`
  (`artigo.tex`, `referencias.bib`, figuras de `scripts/paper_figs.py`, tabelas de
  `scripts/paper_tables.py`). Compilado pelo aluno no Overleaf (não há LaTeX local).
  `artigo_overleaf.zip` = pacote para upload.
- Rascunho da proposta do POC II em `docs/PROPOSTA_POC_II_rascunho.md`.
- Pôster 60 × 90 cm no modelo da Mostra UFMG (11/11/2026): `scripts/poster.py` gera
  `docs/poster/poster_POC_I.pptx` (usa o slide 2 do modelo; o slide 1 tinha o endereço sobre o
  QR code) e uma prévia própria com verificação de transbordamento. Precisa das fontes
  Tomorrow e Titillium Web (`--fonts`); o aluno edita e imprime pelo Google Slides.
- **Decisão do aluno (08/11/2026): as melhorias do modelo ficam para o POC II** (ponderar
  passos decisivos, DAgger, seleção por nº final de cores, novos dados, candidatos
  incrementais); isso está explícito na conclusão do artigo.

Próximos passos:
1. Aluno revisar o artigo, compilar no Overleaf e ajustar à formatação do Moodle.
2. Apresentação final do POC I (material do pitch em `docs/pitch/`).
3. POC II, a partir do rascunho da proposta.

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
              pipeline, storage, io_dimacs, exact, model
tests/        pytest (conftest com grafos de teste)
scripts/      pilot.py (piloto de custo), build_dataset.py (base completa),
              eda.py (análise exploratória), train_poc.py (prova de conceito),
              download_dimacs.py e process_dimacs.py (teste final), paper_figs.py e
              paper_tables.py (artigo), pitch_figs.py, poster.py (pôster)
notebooks/    eda.ipynb (análise exploratória interativa)
results/      saídas de experimentos (pilot/, pilot/v1_rollout_somente_dsatur/, eda/,
              poc_modelo/, dimacs/)
docs/         ESPECIFICACAO_BASE.md; relatorio/ (artigo IEEE); PROPOSTA_POC_II_rascunho.md;
              poster/ (pôster da Mostra);
              pitch/ (roteiro do pitch de 5 min + figuras; slides em
              https://claude.ai/artifact/6ezMnUdk5yvJeiY6gpYA9z)
data/         destino da base completa (ignorada pelo git; regenerável)
```

## Comandos

Ambiente virtual em `.venv/` com as versões exatas do `requirements.txt` (o Python global
da máquina tem outras versões; não usar).

```bash
python -m venv .venv && .venv\Scripts\python -m pip install -r requirements.txt
.venv\Scripts\python -m pytest       # rodar sempre antes de concluir uma alteração
.venv\Scripts\python scripts/build_dataset.py                 # camadas A, S e B
.venv\Scripts\python scripts/build_dataset.py --tiers B --limit 20   # teste rápido
.venv\Scripts\python scripts/eda.py                           # relatório em results/eda/
.venv\Scripts\python scripts/train_poc.py                     # results/poc_modelo/ (~6 min)
.venv\Scripts\jupyter lab notebooks/eda.ipynb                 # análise interativa
.venv\Scripts\python scripts/download_dimacs.py               # instâncias + catálogo (data/dimacs/)
.venv\Scripts\python scripts/process_dimacs.py --report-only  # refaz só o relatório (não reaplica a política)
.venv\Scripts\python scripts/paper_figs.py; .venv\Scripts\python scripts/paper_tables.py
.venv\Scripts\python scripts/pilot.py --per-stratum-a 2 --per-stratum-b 2 --per-stratum-s 3
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
