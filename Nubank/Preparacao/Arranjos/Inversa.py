matriz = []

while len(matriz) < 10:
    try:
        x = int(input())
        matriz.append(x)

    except ValueError:
        print("Digite apenas números inteiros")

matriz_inversa = matriz[::-1]

"""for elemento in matriz_inversa:
    print(elemento)
"""

print(matriz_inversa)
