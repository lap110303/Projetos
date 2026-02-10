def collatz_tamanho(n):
    """Retorna o tamanho da sequência de Collatz para um número n."""
    if n <= 0:
        raise ValueError("O número deve ser um inteiro positivo.")
    
    tamanho = 0
    while n != 1:
        if n % 2 == 0:
            n = n // 2
        else:
            n = 3 * n + 1
        tamanho += 1
    return tamanho + 1  # Inclui o 1 final


def collatz_tamanho_memo(limit):
    """Calcula o tamanho das sequências de Collatz de 1 até limit, com memoização."""
    memo = {1: 1}  # comprimento da sequência para 1 é 1
    
    def length(n):
        orig = n
        seq = []
        while n not in memo:
            seq.append(n)
            if n % 2 == 0:
                n = n // 2
            else:
                n = 3 * n + 1
        total = memo[n]
        for x in reversed(seq):
            total += 1
            memo[x] = total
        return memo[orig]
    
    lengths = {}
    for i in range(1, limit + 1):
        lengths[i] = length(i)
    return lengths


# ---------- Código principal ----------
if __name__ == "__main__":
    limite = 100000
    print(f"Calculando Collatz para números entre 1 e {limite}...")

    lengths = collatz_tamanho_memo(limite)
    max_len = max(lengths.values())
    numeros_max = [n for n, l in lengths.items() if l == max_len]

    print(f"\nMaior comprimento (incluindo o 1 final): {max_len}")
    print(f"Número(s) que atingem esse comprimento: {numeros_max}\n")

    # Mostrar os 10 maiores
    top10 = sorted(lengths.items(), key=lambda x: (-x[1], x[0]))[:10]
    print("Top 10 números com as maiores sequências:")
    for n, l in top10:
        print(f"{n:>6} → {l}")
