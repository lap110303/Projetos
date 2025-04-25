#include <stdio.h>
#include <stdlib.h>

void imprime(int vetor[], int n)
{

    for (int i = 0; i < n; i++)
    {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

void multiplica(int vetor[], int n, double v)
{

    for (int i = 0; i < n; i++)
    {
        vetor[i]*= v;
    }
}


int main()
{

    int tamanho = 0;
    char entrada[100];
    double multiplo = 0.0;

    fgets(entrada, 100, stdin);
    sscanf(entrada, "%d", &tamanho);

    int vetor[tamanho];

    for (int i = 0; i < tamanho; i++)
    {
        fgets(entrada, 100, stdin);
        sscanf(entrada, "%d", &vetor[i]);
    }

    fgets(entrada, 100, stdin);
    sscanf(entrada, "%lf", &multiplo);

    imprime(vetor, tamanho);
    multiplica(vetor, tamanho, multiplo);
    imprime(vetor, tamanho);
    multiplica(vetor, tamanho, 1/multiplo);
    imprime(vetor, tamanho);

    return 0;
}