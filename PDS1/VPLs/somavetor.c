#include <stdio.h>
#include <stdlib.h>

int *ler_criar(int num)
{

    int *v = (int *)malloc(num * sizeof(int));
    char entrada[50];

    for (int i = 0; i < num; i++)
    {
        fgets(entrada, 50, stdin);
        sscanf(entrada, "%d", &v[i]);
    }
    return v;
}

int soma(int vetor[], int tamanho_vetor)
{

    int sum;
    if (tamanho_vetor >= 1) 
    sum = vetor[tamanho_vetor-1] + soma(vetor, tamanho_vetor - 1);
    else return 0;
    return sum;
}

int main()
{
    
    char entrada[50];
    int num = 0;

    fgets(entrada, 50, stdin);
    sscanf(entrada, "%d", &num);

    int *vetor = ler_criar(num);
    int sum = soma(vetor, num);
    printf("%d", sum);

    return 0;
}