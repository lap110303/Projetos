#!/bin/bash
set -eu

# DCC605: Basic shell programming assignment
# Autograding script

srcfn="sh.c"
total=14   # agora temos 14 testes no total
ecnt=0

# Test 0: Code should compile without warnings
if ! tests/test0.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 1: Simple ls command
if ! tests/test1.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 2: Sequence of simple ls commands
if ! tests/test2.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 3: Output redirection command
if ! tests/test3.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 4: Input redirection command
if ! tests/test4.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 5: Input/output redirection command
if ! tests/test5.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 6: Pipe command
if ! tests/test6.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 7: Sequence of pipe commands
if ! tests/test7.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 8: Sequence of pipe commands with output redirection
if ! tests/test8.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 9: Subshell simples
if ! tests/test9.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 10: Subshell com pipe
if ! tests/test10.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 11: Background job
if ! tests/test11.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 12: Wait command
if ! tests/test12.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

# Test 13: Redirecionamento dentro de subshell
if ! tests/test13.sh $srcfn ; then ecnt=$(( $ecnt + 1 )) ; fi

echo "your code passes $(( $total - $ecnt )) of $total tests"
echo "check the output of the getmarks.py script on your code"
exit 0
