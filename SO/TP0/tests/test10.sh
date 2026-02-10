#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

# Subshell + pipe deve funcionar
echo "(echo foo ; echo bar) | sort" | ./sh > tests/sh.tmp

if [ "$(cat tests/sh.tmp)" != $'bar\nfoo' ]; then
    echo "[10] subshell com pipe falhou"
    rm -f tests/sh.tmp
    exit 1
else
    rm -f tests/sh.tmp
    exit 0
fi
