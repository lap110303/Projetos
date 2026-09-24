while True:
    try:
        valor = int(input("Digite o valor: "))
        
        if valor < 0:
            print("Erro, valores menores que 0 não são autorizados")
        else:
            break

    except ValueError:
        print("Apenas números são válidos")
    

notas = [100, 50, 20, 10, 5, 2, 1]

for nota in notas:
    quantidade = valor // nota

    valor = valor % nota

    print(f"{nota}: {quantidade}")