#include <stdio.h>
#include <stdlib.h>

int main()
{
    char nome[10];
    scanf("%s",&nome);
    FILE *arquivo;
    int soma = 0;
    char texto;
    arquivo = fopen(nome, "r");
    
    if(arquivo == NULL)
    {
        system("pause");
        exit(1);
    }
    
    while ((texto = fgetc(arquivo)) != EOF)
    {
        if (texto == 'a'){
        soma ++;}
    }
    
    fclose(arquivo);
    printf ("%d", soma);
    return 0;
}