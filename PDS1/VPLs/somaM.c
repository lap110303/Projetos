#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    char nome[50];
    long int matriz[5][5], matriz2[5][5];
    fgets(nome,50,stdin);
    nome[strlen(nome)-1]='\0';
    FILE *arq_matriz;
    arq_matriz = fopen(nome, "rb");
 
    if (arq_matriz == NULL)
    {
        exit(1);
    }
    for (int i = 0; i < 5; i++)
    {
            for (int j = 0; j < 5; j++)
            {
                fscanf(arq_matriz, "%ld", &matriz[i][j]);
            }
    }
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            fscanf(arq_matriz, "%ld", &matriz2[i][j]);
        }
    }
    for (int i = 0; i < 5; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            printf("%ld ", matriz[i][j] + matriz2[i][j]);
        }
        
        printf("\n");
    }
    
    fclose(arq_matriz);
    return 0;
}