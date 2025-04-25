#include <stdio.h>
#include <stdlib.h>

int **criarmatriz(int linhas, int colunas)
{

    int **matriz = (int **)malloc(linhas * sizeof(int *));

    for (int i = 0; i < linhas; i++)
    {
        matriz[i] = (int *)malloc(colunas * sizeof(int));
        for (int j = 0; j < colunas; j++)
        {
            scanf("%d", & matriz[i][j]);
        }
    }
    return matriz;
}

void liberar_matriz(int **matriz, int linhas)
{

    for (int i = 0; i < linhas; i++)
    {
        free(matriz[i]);
    }
    free(matriz);
}

void imprimir(int **m, int linhas, int colunas)
{
    for (int i = 0; i < linhas; i++)
    {
        for (int j = 0; j < colunas; j++)
        {
            printf("%d ", m[i][j]);
        }
        printf("\n");
    }
}

int main()
{

    int l, c;
    scanf("%d %d", &l, &c);

    int **m = criarmatriz(l, c);
    imprimir(m, l, c);
    liberar_matriz(m, l);

    return 0;
}