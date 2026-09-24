while True:
    try:
        x = int(input("Digite o limite inferior: "))

        if x < 2:
            print("Somente válido números maiores que 1")
        else:
            break

    except ValueError:
        print("Válido apenas números")

while True:
    try:
        y = int(input("Digite o limite superior: "))

        if y < 0 or y > 10000 or y < x:
            print("Somente válido números entre 0 e 10000 e maiores que o limite inferior")
        else:
            break

    except ValueError:
        print("Válido apenas números")

primo = True

for i in range (x, y + 1):
    primo = True
    for j in range (2, i):
        if i % j == 0:
            primo = False
            break

    if primo == True:
        print(i)