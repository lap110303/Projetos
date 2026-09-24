while True:
    try:
        i = int(input())
        j = int(input())
        if i < 0 or j < 0:
            print("Válido apenas números maiores que 0")
        else:
            break
    except:
        print("Válido apenas números")

matriz_oposta = []

for x in range(i):
    linha = list(map(int, input().split()))

    linha_inversa = [-valor for valor in linha]

    matriz_oposta.append(linha_inversa)

for linha in matriz_oposta:
    print(" ".join(map(str, linha)))