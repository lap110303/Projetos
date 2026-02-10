#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

echo "(echo testando) > tests/sh.tmp" | ./sh

if [ "$(cat tests/sh.tmp)" != "testando" ]; then
    echo "[13] redirecionamento dentro do subshell falhou"
    rm -f tests/sh.tmp
    exit 1
else
    rm -f tests/sh.tmp
    exit 0
fi
