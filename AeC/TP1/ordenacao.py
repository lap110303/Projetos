"""
Tradução para Python dos algoritmos de ordenação em C.

Assim como no original, os vetores usam índices de 1 a n.
A posição 0 é reservada para a sentinela usada na Inserção (funcao2),
por isso o vetor tem tamanho MAXTAM + 1.
"""

import random
from dataclasses import dataclass

# Definições gerais para todos os grupos
MAXTAM = 20


@dataclass
class TipoItem:
    chave: int = 0
    # outros componentes


def novo_vetor():
    """Equivalente a TipoVetor A: MAXTAM + 1 posições (sentinela em A[0])."""
    return [TipoItem() for _ in range(MAXTAM + 1)]


# ---------------------------------------------------------------------------
# funcao1 - Seleção
# ---------------------------------------------------------------------------
def funcao1(A, n):
    for i in range(1, n):  # i = 1 .. n-1
        Min = i
        for j in range(i + 1, n + 1):  # j = i+1 .. n
            if A[j].chave < A[Min].chave:
                Min = j
        A[Min], A[i] = A[i], A[Min]


# ---------------------------------------------------------------------------
# funcao2 - Inserção (com sentinela em A[0])
# ---------------------------------------------------------------------------
def funcao2(A, n):
    for i in range(2, n + 1):
        x = A[i]
        j = i - 1
        A[0] = x  # sentinela
        while x.chave < A[j].chave:
            A[j + 1] = A[j]
            j -= 1
        A[j + 1] = x


# ---------------------------------------------------------------------------
# funcao3 - Shellsort (sequência de Knuth: 1, 4, 13, 40, ...)
# ---------------------------------------------------------------------------
def funcao3(A, n):
    h = 1
    # do ... while (h < n)
    while True:
        h = h * 3 + 1
        if not h < n:
            break

    # do ... while (h != 1)
    while True:
        h //= 3
        for i in range(h + 1, n + 1):
            x = A[i]
            j = i
            while A[j - h].chave > x.chave:
                A[j] = A[j - h]
                j -= h
                if j <= h:
                    break  # equivalente ao "goto L999"
            A[j] = x  # L999
        if h == 1:
            break


# ---------------------------------------------------------------------------
# funcao4 - Quicksort
# ---------------------------------------------------------------------------
def particao(Esq, Dir, A):
    """Em C, i e j eram passados por ponteiro; aqui são retornados."""
    i = Esq
    j = Dir
    x = A[(i + j) // 2]  # obtém o pivô x
    # do ... while (i <= j)
    while True:
        while x.chave > A[i].chave:
            i += 1
        while x.chave < A[j].chave:
            j -= 1
        if i <= j:
            A[i], A[j] = A[j], A[i]
            i += 1
            j -= 1
        if not i <= j:
            break
    return i, j


def ordena(Esq, Dir, A):
    i, j = particao(Esq, Dir, A)
    if Esq < j:
        ordena(Esq, j, A)
    if i < Dir:
        ordena(i, Dir, A)


def funcao4(A, n):
    ordena(1, n, A)


# ---------------------------------------------------------------------------
# funcao5 - Heapsort
# ---------------------------------------------------------------------------
def refaz(Esq, Dir, A):
    i = Esq
    j = i * 2
    x = A[i]
    while j <= Dir:
        if j < Dir:
            if A[j].chave < A[j + 1].chave:
                j += 1
        if x.chave >= A[j].chave:
            break  # equivalente ao "goto L999"
        A[i] = A[j]
        i = j
        j = i * 2
    A[i] = x  # L999


def constroi(A, n):
    Esq = n // 2 + 1
    while Esq > 1:
        Esq -= 1
        refaz(Esq, n, A)


def funcao5(A, n):
    constroi(A, n)  # constrói o heap
    Esq = 1
    Dir = n
    while Dir > 1:  # ordena o vetor
        A[1], A[Dir] = A[Dir], A[1]
        Dir -= 1
        refaz(Esq, Dir, A)


# ---------------------------------------------------------------------------
# Teste rápido
# ---------------------------------------------------------------------------
if __name__ == "__main__":
    n = MAXTAM
    funcoes = [
        ("funcao1 (Seleção)", funcao1),
        ("funcao2 (Inserção)", funcao2),
        ("funcao3 (Shellsort)", funcao3),
        ("funcao4 (Quicksort)", funcao4),
        ("funcao5 (Heapsort)", funcao5),
    ]

    chaves = [random.randint(0, 99) for _ in range(n)]
    print("Original:", chaves)

    for nome, f in funcoes:
        A = novo_vetor()
        for i in range(1, n + 1):
            A[i] = TipoItem(chaves[i - 1])
        f(A, n)
        resultado = [A[i].chave for i in range(1, n + 1)]
        ok = resultado == sorted(chaves)
        print(f"{nome:22} {resultado} {'OK' if ok else 'ERRO'}")
