#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    FILE *arquivo;
    int soma = 0;
    char texto;
    arquivo = fopen(argv[1], "r");
    
    if(arquivo == NULL)
        {
            system("pause");
            exit(1);
        }
        
    while ((texto = fgetc(arquivo)) != EOF)
    {
        if (texto == 'a')
        {
            soma ++;
        }
    }
    
    fclose(arquivo);
    printf ("%d", soma);
    return 0;
 }