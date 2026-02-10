#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

# Cria processo em background e depois espera
start=$(date +%s)
{ echo "sleep 2 &"; echo "wait"; } | ./sh
end=$(date +%s)

# agora deve esperar ~2 segundos
if [ $((end - start)) -lt 2 ]; then
    echo "[12] wait não esperou os processos em background"
    exit 1
else
    exit 0
fi
