# Roteiro do pitch — POC I (5 minutos)

Guia para apresentar e gravar um pitch curto do que foi feito no POC I até outubro de 2026:
base de dados, análise exploratória e prova de conceito do modelo.

## Materiais

| Material | Onde está | Uso |
|---|---|---|
| Apresentação (10 slides) | https://claude.ai/artifact/6ezMnUdk5yvJeiY6gpYA9z | Mostrar na tela, em modo de apresentação. As falas abaixo também estão nas notas de cada slide. |
| Figuras do pitch | `docs/pitch/figs/` (`vencedora.png`, `rotulo_por_passo.png`) | Já estão nos slides 5 e 6. Para gerar de novo: `.venv\Scripts\python scripts/pitch_figs.py`. |
| Notebook interativo | `notebooks/eda.ipynb` | Demonstração opcional (seção "Demonstração opcional"). |
| Relatórios completos | `results/eda/RELATORIO_EDA.md`, `results/poc_modelo/RELATORIO_MODELO.md` | Fonte de todos os números; úteis para as perguntas. |

**Compartilhamento:** o link da apresentação é privado. Para outras pessoas abrirem, use o menu
de compartilhamento da própria página. Também dá para baixar em PDF ou PPTX pelo mesmo menu e
anexar ao e-mail junto com o vídeo.

## Mapa do tempo

| # | Slide | Tempo | Mensagem-chave |
|---|---|---|---|
| 1 | Capa | 0:00–0:15 | Quem sou e o que vou mostrar |
| 2 | O problema | 0:15–0:55 | A ordem decide as cores; nenhuma heurística vence sempre |
| 3 | O que construímos | 0:55–1:20 | Quatro etapas: heurísticas, base, análise, modelo |
| 4 | A base de dados | 1:20–1:50 | Camadas A, B e S; DIMACS guardado para o teste final |
| 5 | Como rotulamos | 1:50–2:30 | Rollout: o oráculo vence a melhor heurística em 55% dos grafos |
| 6 | O que os dados mostram | 2:30–3:05 | A família do grafo decide quem vence; só 59% dos passos decidem |
| 7 | Prova de conceito | 3:05–3:35 | Por passo, o modelo acerta 97% |
| 8 | O resultado que importa | 3:35–4:20 | De ponta a ponta, empata; por quê |
| 9 | Próximos passos | 4:20–4:50 | Três melhorias para o POC II; DIMACS e relatório |
| 10 | Fechamento | 4:50–5:00 | Reprodutível; perguntas |

Ritmo: cerca de 670 palavras em 5 minutos (≈ 135 por minuto). Se estiver passando do tempo,
encurte o slide 2 (pule a frase sobre NP-difícil) e o slide 4 (cite só as camadas A e B).

## Roteiro falado

### 1. Capa (0:00–0:15)
> Olá, eu sou o Lucas Affonso Pires e este é o meu POC I, orientado pelo professor Marcio Costa
> Santos. Em cinco minutos, vou mostrar o que construímos até aqui e o que os resultados já nos
> ensinaram.

Olhe para a câmera nesta fala.

### 2. O problema (0:15–0:55)
> Colorir um grafo é dar uma cor a cada vértice sem que vizinhos fiquem com a mesma cor, usando o
> mínimo possível de cores. Como achar o ótimo é NP-difícil, na prática usamos algoritmos gulosos:
> colorimos um vértice por vez, sempre com a menor cor permitida. O que muda o resultado é a ordem.
> O Welsh-Powell ordena pelo grau; o DSATUR escolhe o vértice com mais cores diferentes ao redor;
> o RLF monta uma cor de cada vez. Nenhum vence sempre. A pergunta do trabalho é: um modelo
> consegue aprender, passo a passo, qual vértice colorir, usando no máximo tantas cores quanto
> essas heurísticas?

Aponte os três cartões em ordem; termine na faixa laranja (a pergunta).

### 3. O que construímos (0:55–1:20)
> O trabalho teve quatro etapas. Implementamos as quatro heurísticas num mesmo esquema, com 41
> testes automáticos. Construímos uma base com 2.818 grafos e quase 860 mil candidatos rotulados.
> Fizemos a análise exploratória, com relatório automático e um notebook interativo. E fechamos
> com uma prova de conceito do modelo.

### 4. A base de dados (1:20–1:50)
> A base tem camadas. A camada A, com 2.755 grafos Erdős–Rényi e Barabási–Albert de até mil
> vértices, diz qual heurística vence em cada grafo. A camada B, um subconjunto com até 500
> vértices, traz o rótulo de cada passo. A camada S testa grafos de até dez mil vértices. As
> instâncias DIMACS ficam guardadas para o teste final, usadas uma única vez. Tudo é reprodutível
> e a base inteira foi gerada em 26 minutos.

### 5. Como rotulamos cada passo (1:50–2:30)
> O ponto central é como rotular cada passo. A cada passo, cada heurística sugere um vértice. Para
> cada sugestão, completamos a coloração com o DSATUR e com o RLF e contamos as cores. As sugestões
> que levam ao menor número recebem rótulo 1. Neste grafo, sozinho, o RLF usa 57 cores e o DSATUR
> 61; seguindo sempre a melhor sugestão, o que chamamos de oráculo, chegamos a 53. Em 55% dos
> grafos o oráculo supera a melhor heurística, e nunca fica pior. Ou seja: há espaço para aprender.

No gráfico, mostre as duas linhas pontilhadas (57 e 61) e depois a linha escura descendo até 53.

### 6. O que os dados mostram (2:30–3:05)
> A análise exploratória mostrou três coisas. Primeiro, a disputa é só entre RLF e DSATUR: o RLF
> vence nos grafos Erdős–Rényi, em laranja, e o DSATUR nos Barabási–Albert, em azul. Segundo, a
> família do grafo praticamente decide a vencedora. Terceiro, só 59% dos passos têm uma escolha
> errada possível; nos outros, qualquer sugestão dá o mesmo resultado. Por isso o modelo é treinado
> só nesses passos decisivos.

### 7. Prova de conceito (3:05–3:35)
> Na prova de conceito, um classificador escolhe, entre os candidatos de cada passo, o que tem mais
> chance de ser ótimo. Treinamos em 650 grafos e validamos em 163 outros, sem misturar grafos entre
> treino e validação. O gradient boosting acerta 97% das escolhas, bem acima de seguir o RLF, com
> 89%, ou da melhor regra simples, com 90%.

### 8. O resultado que importa (3:35–4:20)
> Só que o objetivo é usar menos cores no grafo inteiro. E aqui o resultado é honesto: colorindo de
> ponta a ponta, o modelo empata com a melhor heurística em 96% dos grafos. Em grafos que ele nunca
> viu, de até mil vértices, ganha em 2% e perde em 7%. O motivo: ele aprendeu a seguir o RLF nos
> grafos Erdős–Rényi e o DSATUR nos Barabási–Albert, ou seja, virou um bom seletor de heurística.
> E os poucos erros se acumulam: 2,8% de erro em uns 195 passos decisivos dá umas 5 escolhas
> erradas por grafo, mais do que o ganho possível, de pouco mais de uma cor.

É o slide mais importante: fale devagar e faça uma pausa depois de "empata".

### 9. Próximos passos (4:20–4:50)
> Isso define os próximos passos. Para o POC II: dar mais peso aos passos que realmente mudam o
> número de cores; treinar também nos estados que a própria política visita, a técnica chamada
> DAgger; e otimizar o número final de cores, não a acurácia por passo. Para fechar o POC I: o
> teste final nas instâncias DIMACS, que ficaram guardadas até agora, e o relatório técnico.

### 10. Fechamento (4:50–5:00)
> Tudo está reprodutível: a base, os 41 testes, os relatórios e o notebook. Obrigado, fico à
> disposição para perguntas e sugestões.

Olhe para a câmera nesta fala.

## Demonstração opcional (+30 s, ou no lugar do gráfico do slide 5)

Mostra ao vivo, no notebook, o mesmo exemplo do slide 5, e prova que tudo sai direto dos dados.

**Antes de gravar:**
1. Abra o notebook: `.venv\Scripts\jupyter lab notebooks/eda.ipynb` (a partir da pasta POC-I).
2. Rode tudo uma vez (*Run → Run All Cells*, menos de um minuto) e role até a seção 6,
   célula "Explore: os candidatos passo a passo".
3. Deixe a janela do Jupyter aberta atrás do navegador da apresentação.

**Durante o slide 5:** troque para o Jupyter, altere a primeira linha da célula para
`GRAPH_ID_B = "er_n150_p0.90_s0"`, rode com Shift+Enter e diga:

> Este é o mesmo grafo, direto da base. Cada linha é o valor da sugestão de uma heurística; a
> linha tracejada é o oráculo; as faixas cinza são os passos em que errar custa cores.

Volte para a apresentação. Se usar a demonstração, encurte a fala do slide 5 para não passar de
5:30 no total.

## Perguntas prováveis (respostas curtas)

| Pergunta | Resposta |
|---|---|
| O modelo não está só copiando o RLF? | Em boa parte, sim, e isso é um achado: ele aprendeu a escolher a heurística certa para cada família (RLF no ER, DSATUR no BA). As três melhorias do slide 9 atacam exatamente isso. |
| Por que não uma rede neural em grafos (GNN)? | A proposta previa um modelo simples na prova de conceito. O gargalo atual (erros que se acumulam e estados diferentes dos do treino) não se resolve só trocando o modelo; GNN é candidata para o POC II. |
| Por que completar o rollout com DSATUR **e** RLF? | Na versão 1, só com DSATUR, o oráculo ficava pior que a melhor heurística em 33% dos grafos. Com os dois, há garantia (melhoria sequencial, Bertsekas) de nunca ficar pior que o melhor deles. |
| Houve vazamento entre treino e validação? | Não: a divisão é por grafo, a generalização foi medida em 551 grafos nunca vistos (réplica 3) e o DIMACS não foi tocado. |
| Quanto custa a política aprendida? | Cerca de 4,7 s por grafo com mais de 500 vértices, contra menos de 0,1 s do RLF, porque a cada passo calcula as 4 sugestões e consulta o modelo. Dá para calcular as sugestões de forma incremental no POC II. |
| Por que só grafos ER e BA? | Permitem controlar tamanho e densidade. A grade original (até 10.000 vértices em todas as combinações) era inviável, com cerca de 10¹² arestas. O DIMACS vai testar outras estruturas. |
| O que é um "passo decisivo" (informativo)? | Um passo em que ao menos uma sugestão leva a mais cores que a melhor; nos outros, qualquer escolha dá o mesmo resultado. |

## Gravação: tela + câmera do notebook

### Ferramenta recomendada: Microsoft Clipchamp
É o editor de vídeo que já vem no Windows 11 (se não estiver instalado, é gratuito na Microsoft
Store). Grava a tela e a webcam ao mesmo tempo, deixa a câmera como uma camada separada que dá para
mover e redimensionar, permite cortar erros na linha do tempo e exporta MP4 em 1080p.

Passo a passo (os nomes dos menus podem variar um pouco entre versões):
1. Abra o Clipchamp pelo menu Iniciar e entre com sua conta Microsoft.
2. Clique em **Criar um novo vídeo**.
3. Na barra lateral, abra **Gravar e criar** e escolha **Tela e câmera**.
4. Autorize câmera e microfone e selecione a webcam e o microfone corretos.
5. Clique em gravar e escolha **a tela inteira** (assim dá para alternar entre a apresentação e o
   Jupyter, se fizer a demonstração).
6. Apresente; ao terminar, pare a gravação.
7. Na linha do tempo, a tela e a câmera aparecem em faixas separadas. Reduza a câmera para cerca
   de 1/5 da largura e posicione no canto **inferior direito** (cobre só o número da página).
8. Corte o início e o fim e exporte em **1080p**.

### Alternativas
- **OBS Studio** (gratuito, código aberto): mais controle e sem edição. Monte uma cena com
  "Captura de tela" e "Dispositivo de captura de vídeo" (a webcam) num canto e grave. Se gravar em
  MKV, use *Arquivo → Remux de gravações* para gerar o MP4.
- **PowerPoint** (se tiver Microsoft 365): baixe a apresentação em PPTX e use a gravação de slides
  com câmera. As fontes podem ser trocadas por outras parecidas se não estiverem instaladas.
- **Loom**: gera um link na hora, mas o plano gratuito costuma limitar a duração dos vídeos
  (historicamente a 5 minutos), arriscado para este pitch. Confira o limite atual antes de usar.

### Checklist antes de gravar
- [ ] Ensaiar duas vezes com cronômetro (meta: 4:45 a 5:00).
- [ ] Apresentação aberta no navegador, em modo de apresentação, em tela cheia.
- [ ] Ativar o "Não incomodar" do Windows e fechar e-mail, chat e abas desnecessárias.
- [ ] Luz de frente (janela ou luminária atrás da câmera, nunca atrás de você).
- [ ] Câmera na altura dos olhos; enquadramento do peito para cima.
- [ ] Fone com microfone, se tiver: melhora muito o áudio. Fazer um teste de 15 s e ouvir.
- [ ] Notebook na tomada (a gravação consome bateria e o Windows pode reduzir o desempenho).

### Envio
Um vídeo de 5 minutos em 1080p costuma ter algumas dezenas a pouco mais de cem megabytes, grande
demais para anexo de e-mail. Envie por link: a pasta do projeto já está no OneDrive (botão direito
→ *Compartilhar*), ou use Google Drive ou YouTube "não listado". Mande junto o PDF da apresentação.
