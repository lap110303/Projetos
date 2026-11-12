#!/usr/bin/env bash
# Reproduz todo o pipeline: dados -> tokens -> Markov -> Transformer -> geração -> avaliação.
# Uso: bash run_all.sh   (com o ambiente virtual ativado; PY=python por padrão)
set -euo pipefail
PY=${PY:-python}
mkdir -p logs

$PY scripts/prepare_data.py                     | tee logs/prepare_data.log
$PY scripts/train_markov.py                     | tee logs/train_markov.log
for DO in 0.15 0.3; do
  TAG=do${DO/./}
  $PY -u scripts/train_transformer.py --dropout $DO \
      --out checkpoints/transformer_$TAG.pt \
      --history results/transformer_history_$TAG.json | tee logs/train_transformer_$TAG.log
done
$PY scripts/select_model.py                     | tee logs/select_model.log
$PY -u scripts/generate.py --system both --n 200 | tee logs/generate.log
$PY scripts/evaluate.py                         | tee logs/evaluate.log
$PY scripts/make_figures.py
$PY scripts/render_showcase.py                  | tee logs/render_showcase.log
$PY scripts/analyze_tonality.py                 | tee logs/analyze_tonality.log
