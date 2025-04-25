#include <stdio.h>
#include <stdlib.h>

void somarvetor(int *v, int *v1, int *soma, int tamanho)
{

    for (int i = 0; i < tamanho; i++)
    {
        soma[i] = v[i] + v1[i];
    }
}

void lervetor(int *v, int tamanho)
{

    for (int i = 0; i < tamanho; i++)
    {

        scanf("%d", &v[i]);
    }
}

void imprimir(int *v, int tamanho)
{

    for (int i = 0; i < tamanho; i++)
    {

        printf("%d\n", v[i]);
    }
}

int main()
{

    int tamanho;
    scanf("%d", &tamanho);

    int *v = (int *)malloc(tamanho * sizeof(int));
    int *v1 = (int *)malloc(tamanho * sizeof(int));
    int *vsoma = (int *)malloc(tamanho * sizeof(int));

    lervetor(v, tamanho);
    lervetor(v1, tamanho);

    somarvetor(v, v1, vsoma, tamanho);

    imprimir(vsoma, tamanho);

    return 0;
}