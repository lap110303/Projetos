while True:
    try:
        n = int(input("Digite n: "))

        if n < 2:
            print("Somente válido valores maiores ou iguais a 2")
        else:
            break
    except ValueError:
        print("Somente valores numéricos aceitos")

while True:
    try:
        x0 = int(input("Digite x0: "))
        break
    except ValueError:
        print("Somente valores numéricos aceitos")

while True:
    try:
        x1 = int(input("Digite x1: "))
        break
    except ValueError:
        print("Somente valores numéricos aceitos")

print(f"x0: {x0}")
print(f"x1: {x1}")

x_anterior = x1
x_anterior2 = x0



for i in range (2, n + 1):
    x_atual = 4 * x_anterior - 2 * x_anterior2

    print(f"x{i}: {x_atual}")
    
    x_anterior2 = x_anterior
    x_anterior = x_atual
