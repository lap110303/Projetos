# bach-markov — Corais no estilo de J. S. Bach com Cadeias de Markov

Sistema de geração musical simbólica de ponta a ponta: do corpus de corais de Bach
(`music21.corpus`) até arquivos **MIDI / MusicXML** reproduzíveis e áudio sintetizado.

- **Método:** exclusivamente cadeias de Markov (a opção (c) do trabalho).
  - *Melodia:* cadeia de ordem **n** (configurável) sobre eventos do soprano
    `(altura, duração, posição métrica, fermata)`, com probabilidades de transição
    **aprendidas por máxima verossimilhança** no corpus, temperatura `tau` e *back-off*.
  - *Harmonia:* cadeia de ordem 1 sobre sonoridades verticais SATB, amostrada
    **condicionada** à melodia gerada (restrições de Markov, amostragem exata por
    *forward-filtering / backward-sampling*), com suavização `alpha`.
- **Restrição estilística:** coral luterano a quatro vozes no estilo de J. S. Bach
  (4/4, frases delimitadas por fermatas, cadência final na tônica).

## Instalação

```bash
git clone https://github.com/<SEU-USUARIO>/bach-markov.git
cd bach-markov
python -m venv .venv && source .venv/bin/activate
pip install -r requirements.txt
```

Para gerar áudio (opcional): instale o [FluidSynth](https://www.fluidsynth.org/) e um
*soundfont* General MIDI livre (ex.: FluidR3_GM), além do `ffmpeg` para MP3.

```bash
sudo apt install fluidsynth fluid-soundfont-gm ffmpeg   # Debian/Ubuntu
brew install fluid-synth ffmpeg                         # macOS (+ baixe um .sf2)
export SOUNDFONT=/caminho/para/soundfont.sf2            # se não estiver no local padrão
```
Sem FluidSynth, o código recorre à síntese senoidal do `pretty_midi` (WAV).

## Uso

```bash
# 1) (opcional) reconstruir o cache do corpus — data/chorales.json já vem no repositório
python -m bachmarkov.corpus

# 2) gerar corais: várias sementes com a MESMA configuração
python scripts/generate.py --order 3 --seeds 1 2 3 --audio

# 3) variar hiperparâmetros
python scripts/generate.py --order 1 --seeds 1 --audio            # ordem baixa
python scripts/generate.py --order 6 --seeds 1 --audio            # ordem alta (memoriza)
python scripts/generate.py --order 2 --tau 1.5 --mode minor --key D --seeds 3 --audio
python scripts/generate.py --order 3 --alpha 100 --seeds 1        # harmonia "sem memória"
```

| Parâmetro   | Descrição                                               | Padrão |
|-------------|---------------------------------------------------------|--------|
| `--order`   | ordem *n* da cadeia melódica                            | 3      |
| `--tau`     | temperatura de amostragem (p ∝ c^(1/τ))                 | 1.0    |
| `--alpha`   | suavização da cadeia harmônica                          | 0.1    |
| `--mode`    | `major` ou `minor` (seleciona o sub-corpus)             | major  |
| `--key`     | tonalidade de saída (C, G, D, Bb…)                      | C / A  |
| `--seeds`   | sementes (mesma semente ⇒ mesmo resultado)              | 0      |
| `--min-len` / `--max-len` | limites de nº de notas (amostragem por rejeição) | 32 / 80 |

Saídas em `outputs/`: `midi/*.mid`, `musicxml/*.musicxml` (abre no MuseScore),
`json/*.json` (eventos + metadados) e `audio/*.mp3`.

## Reprodução dos resultados do artigo

```bash
python scripts/model_stats.py        # esparsidade do modelo por ordem
python scripts/experiments.py        # varreduras de n, tau e alpha (30 sementes cada)
                                     #  -> results/*.csv e figures/sweeps.pdf
python scripts/make_showcase.py      # peças de demonstração -> outputs/ + showcase.csv
```

## Peças geradas (áudio em `outputs/audio/`)

| Peça | n | τ | α | Modo/tonalidade | O que ilustra |
|------|---|---|---|-----------------|---------------|
| [coral01](outputs/audio/coral01.mp3) | 1 | 1.0 | 0.1 | Dó maior | ordem baixa: frases irregulares |
| [coral02](outputs/audio/coral02.mp3) | 3 | 1.0 | 0.1 | Dó maior | configuração principal |
| [coral03](outputs/audio/coral03.mp3) | 3 | 1.0 | 0.1 | Dó maior | mesma configuração, outra semente |
| [coral04](outputs/audio/coral04.mp3) | 2 | 1.5 | 0.1 | Ré menor | modo menor, temperatura alta |
| [coral05](outputs/audio/coral05.mp3) | 6 | 1.0 | 0.1 | Dó maior | ordem alta: cópia integral do BWV 399 |
| [coral06](outputs/audio/coral06.mp3) | 3 | 1.0 | 100 | Dó maior | mesma melodia do coral02, harmonia sem memória |

## Estrutura

```
bachmarkov/
  corpus.py     extração do corpus (music21), transposição para Dó maior / Lá menor
  markov.py     MelodyMarkov (ordem n) e HarmonyMarkov (ordem 1 condicionada)
  generate.py   pipeline melodia -> harmonização
  export.py     MIDI (pretty_midi), MusicXML (music21), áudio (FluidSynth)
  evaluate.py   métricas: memorização, fidelidade, diversidade, paralelas
scripts/        CLI de geração e experimentos
paper/          artigo ISMIR (LaTeX, pronto para Overleaf)
```

## Referências principais
- Cuthbert & Ariza (2010). *music21: A toolkit for computer-aided musicology*. ISMIR.
- Pachet & Roy (2011). *Markov constraints: steerable generation of Markov sequences*. Constraints.
- Ames (1989). *The Markov process as a compositional model*. Leonardo.
