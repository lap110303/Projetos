#!/bin/bash
set -eu

if [ ! -s sh ] ; then exit 1 ; fi

# sleep deve rodar em background, liberando o prompt antes de terminar
start=$(date +%s)
echo "sleep 2 &" | ./sh
end=$(date +%s)

# se o shell esperar, a diferença será >=2; se liberar o prompt, será <2
if [ $((end - start)) -ge 2 ]; then
    echo "[11] background job não liberou o prompt"
    exit 1
else
    exit 0
fi
