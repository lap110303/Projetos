#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

# O comando entre parênteses deve executar e salvar no arquivo
echo "(echo ola)" | ./sh > tests/sh.tmp

if [ "$(cat tests/sh.tmp)" != "ola" ]; then
    echo "[9] subshell simples falhou"
    rm -f tests/sh.tmp
    exit 1
else
    rm -f tests/sh.tmp
    exit 0
fi
