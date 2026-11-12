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
│   ├── model.py            # prova de conceito: atributos dos candidatos e política aprendida
│   ├── io_dimacs.py        # leitura/escrita do formato DIMACS (.col e binário .col.b)
│   └── exact.py            # número cromático exato (só para testes)
├── tests/                  # testes automatizados (pytest)
├── scripts/
│   ├── pilot.py            # piloto de tempo, memória e disco
│   ├── build_dataset.py    # geração da base completa (paralela, retomável)
│   ├── eda.py              # análise exploratória (relatório + figuras)
│   ├── train_poc.py        # treino e avaliação da prova de conceito do modelo
│   ├── download_dimacs.py  # instâncias DIMACS e catálogo (data/dimacs/)
│   ├── process_dimacs.py   # teste final no DIMACS (uso único da política)
│   ├── paper_figs.py       # figuras do artigo (docs/relatorio/figs/)
│   ├── paper_tables.py     # tabelas do artigo (docs/relatorio/tabelas/)
│   ├── pitch_figs.py       # figuras do pitch (docs/pitch/figs/)
│   └── poster.py           # pôster 60 × 90 cm no modelo da Mostra UFMG (docs/poster/)
├── notebooks/
│   └── eda.ipynb           # análise exploratória interativa (mesmo código do eda.py)
├── results/pilot/          # saídas do piloto (CSV, JSON, relatório)
├── results/eda/            # relatório da análise exploratória, figuras e tabelas
├── results/poc_modelo/     # relatório da prova de conceito, figuras, tabelas e modelo
├── results/dimacs/         # teste final no DIMACS: relatório, figuras, tabela completa
├── docs/
│   ├── ESPECIFICACAO_BASE.md
│   ├── relatorio/          # artigo final (modelo IEEE): artigo.tex, referencias.bib, figs/, tabelas/
│   ├── PROPOSTA_POC_II_rascunho.md
│   ├── pitch/              # roteiro do pitch de 5 min (ROTEIRO_PITCH.md) e figuras
│   └── poster/             # pôster (poster_POC_I.pptx), prévia, figuras e o modelo do banner
├── data/                   # base completa (fora do git; regenerável)
├── CLAUDE.md               # contexto do projeto para o Claude Code
└── requirements.txt
```

## Como usar

```bash
cd POC-I
pip install -r requirements.txt
python -m pytest              # 43 testes
python scripts/pilot.py --per-stratum-a 2 --per-stratum-b 2 --per-stratum-s 3
python scripts/build_dataset.py                    # base completa em data/
python scripts/eda.py                              # análise exploratória em results/eda/
python scripts/train_poc.py                        # prova de conceito em results/poc_modelo/
jupyter lab notebooks/eda.ipynb                    # análise exploratória interativa
python scripts/download_dimacs.py                  # 127 instâncias DIMACS + catálogo
python scripts/process_dimacs.py                   # teste final (uso único) em results/dimacs/
python scripts/paper_figs.py && python scripts/paper_tables.py   # insumos do artigo
python scripts/poster.py --fonts <pasta>           # pôster da Mostra (precisa das fontes do modelo)
```

**Pôster.** `scripts/poster.py` preenche o modelo do banner da Mostra UFMG (60 × 90 cm) e
gera `docs/poster/poster_POC_I.pptx`, as figuras e uma prévia (`previa.png`) desenhada a partir
da mesma especificação, que também acusa texto transbordando. Precisa das fontes Tomorrow e
Titillium Web (Google Fonts) numa pasta passada em `--fonts`. Abra e imprima pelo Google
Slides, que tem as duas fontes.

**Teste final no DIMACS (uso único).** O `process_dimacs.py` grava o resultado de cada
instância em `data/dimacs/resultados/` com o sha256 do modelo e nunca reprocessa uma
instância já feita; `--report-only` refaz apenas o relatório. O teste foi feito em
08/11/2026 e não deve ser repetido com outro modelo sem registrar isso como novo experimento.

**Artigo final.** `docs/relatorio/artigo.tex` (classe IEEEtran) usa as figuras e tabelas
geradas pelos scripts acima. Para compilar no Overleaf, envie `docs/relatorio/artigo_overleaf.zip`
(*New Project → Upload Project*); localmente: `latexmk -pdf artigo.tex`.

O `eda.py` lê a base de `data/` (sem o DIMACS) e gera `results/eda/RELATORIO_EDA.md`,
com as figuras em `figs/`, as tabelas em `tabelas/` (CSV) e os números-chave em
`resumo.json`. Todo o texto com números é gerado a partir dos dados.

O notebook `notebooks/eda.ipynb` mostra o mesmo relatório dentro do Jupyter (as funções
de seção do `eda.py` exibem tabelas e figuras na hora) e acrescenta, em cada seção, uma
explicação de como ler os resultados e células de exploração com parâmetros editáveis
(ex.: escolher um grafo e ver sua trajetória passo a passo). Ele não grava arquivos.

O `train_poc.py` treina a prova de conceito (regressão logística e gradient boosting sobre
os candidatos dos passos informativos da camada B, divisão por grafo), avalia por passo e
de ponta a ponta (a política aprendida colore os grafos de validação e os grafos da
camada A com réplica 3, que não estão na camada B) e grava o relatório
`results/poc_modelo/RELATORIO_MODELO.md` e o modelo escolhido (`modelo.joblib`).

O `build_dataset.py` processa as camadas A, S e B em paralelo (`--workers`, padrão:
metade dos núcleos lógicos; a camada S usa no máximo 4 por causa da memória). Pode ser
interrompido com Ctrl+C e executado de novo: retoma do ponto em que parou. O progresso
fica em `data/build_log.txt` e as contagens por camada em `data/manifest.json`.

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
- Atributos globais conferem com networkx; gravação e leitura de grafos e DIMACS, inclusive
  o formato binário `.col.b` (ida e volta; conferido também contra as versões em texto das
  instâncias DSJC125.1 e DSJC500.5).
- Construção da base: retomada sem duplicatas, remoção de partes de lotes interrompidos,
  grafos com falha não gravados e refeitos na execução seguinte, camada B em paralelo.
- Análise exploratória: roda de ponta a ponta sobre uma base minúscula e gera as 7 seções.
- Modelo: os atributos calculados pela política (um passo por vez) são idênticos aos do
  treino (tabela de rollouts); uma política que sempre segue o candidato do RLF (ou do
  DSATUR) reproduz exatamente essa heurística; a política treinada é determinística e
  produz coloração válida.
- Teste final: o pipeline do DIMACS roda sobre uma cópia fictícia e uma segunda execução não
  reprocessa nem altera resultados (uso único).
