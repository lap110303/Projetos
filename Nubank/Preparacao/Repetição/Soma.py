while True:
    try:
        x = int(input("Digite o número: "))

        if x <= 0:
            print("Apenas números positivos maiores que 0")
        else:
            break

    except ValueError:
        print("Apenas números")

valor = 0

for i in range(1, x+1):
    valor += i

print(valor)
