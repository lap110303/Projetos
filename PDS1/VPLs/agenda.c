#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Cadastro
{
    char nome[50];
    int idade;
    char telefone[20];
} Cadastro;

int main()
{
    char comando[15], nome_arquivo[15], busca[50];
    int i, t;
    Cadastro agenda[100];
    FILE *arq;
    for (int p = 0; p < 100; p++)
    {
        strcpy(agenda[p].nome, "vazio");
    }
    fgets(nome_arquivo,15,stdin);
    nome_arquivo[strlen(nome_arquivo)-1]='\0';
    arq = fopen(nome_arquivo, "rb");
    if (arq == NULL)
    {
        exit(1);
    }
    fread(agenda, sizeof(Cadastro), 100, arq);
    while (scanf("%s", comando) != EOF)
    {
        if (strcmp(comando, "Inserir") == 0)
        {
            i = 0;
            while (strcmp(agenda[i].nome, "vazio") != 0)
            {
                i++;
                if (i == 100)
            {
                printf("Espaco insuficiente\n");
                break;
            }
        }
        if (i < 100)
        {
            scanf("%s", agenda[i].nome);
            scanf("%d", &agenda[i].idade);
            scanf("%s", agenda[i].telefone);
            printf("Registro %s %d %s inserido\n", agenda[i].nome, agenda[i].idade,
            agenda[i].telefone);
        }
    }
    if (strcmp(comando, "Alterar") == 0)
    {
        i = 0;
        scanf("%s", busca);
        while (i < 100)
        {
            if (strcmp(agenda[i].nome, busca) == 0)
            {
                scanf("%d", &agenda[i].idade);
                scanf("%s", agenda[i].telefone);
                printf("Registro %s %d %s alterado\n", agenda[i].nome, agenda[i].idade,
                agenda[i].telefone);
                break;
            }
            i++;
        }
        if (i == 100)
        {
            printf("Registro %s invalido\n", busca);
        }
    }
    if (strcmp(comando, "Excluir") == 0)
    {
        i = 0;
        t = 0;
        scanf("%s", busca);
        while (i < 100)
        {
            if (strcmp(agenda[i].nome, busca) == 0)
            {
                if (t == 0)
                {
                    printf("Registro %s %d %s excluido\n", agenda[i].nome, agenda[i].idade,
                    agenda[i].telefone);
                }
            t = 1;
            strcpy(agenda[i].nome, "vazio");
            }
            i++;
        }
        if (i == 100 && t == 0)
        {
            printf("Registro %s invalido\n", busca);
        }
    }
 if (strcmp(comando, "Exibir") == 0)
    {
        i = 0;
        scanf("%s", busca);
        while (i < 100)
        {
            if (strcmp(agenda[i].nome, busca) == 0)
            {
                printf("Registro %s %d %s exibido\n", agenda[i].nome, agenda[i].idade,
                agenda[i].telefone);
                break;
            }
            i++;
        }
        if (i == 100)
        {
            printf("Registro %s invalido\n", busca);
        }
    }
}
 arq = fopen(nome_arquivo, "wb");
 fwrite(&agenda, sizeof(struct Cadastro), 100, arq);
 fclose(arq);
 return 0;
}