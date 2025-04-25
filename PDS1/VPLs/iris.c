#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct
{
    float cs;
    float ls;
    float cp;
    float lp;
    char tipo[52];
} iris;

typedef struct
{
    float cs;
    float ls;
    float cp;
    float lp;
} iris_desconhecido;

void leitura(int n, iris flor[])
{
    char entrada[100];
    for (int i = 0; i < n; i++)
    {
        fgets(entrada, 100, stdin);
        sscanf(entrada, "%f %f %f %f %s",
               &flor[i].cs,
               &flor[i].ls,
               &flor[i].cp,
               &flor[i].lp,
               flor[i].tipo);
    }
}

int classificar(iris nao_identificada, int n, iris registros_identificados[])
{

    float indice_soma1 = 0.0, indice_soma = 0.0;
    int posicao_final = 0;

    indice_soma =
        pow(registros_identificados[0].cs - nao_identificada.cs, 2) +
        pow(registros_identificados[0].ls - nao_identificada.ls, 2) +
        pow(registros_identificados[0].cp - nao_identificada.cp, 2) +
        pow(registros_identificados[0].lp - nao_identificada.lp, 2);

    for (int i = 1; i < n; i++)
    {
        indice_soma1 =
            pow(registros_identificados[i].cs - nao_identificada.cs, 2) +
            pow(registros_identificados[i].ls - nao_identificada.ls, 2) +
            pow(registros_identificados[i].cp - nao_identificada.cp, 2) +
            pow(registros_identificados[i].lp - nao_identificada.lp, 2);

        if (indice_soma1 < indice_soma)
        {
            indice_soma = indice_soma1;
            posicao_final = i;
        }
    }
    return posicao_final;
}

int main()
{

    int n = 0;

    char entrada[100];
    fgets(entrada, 100, stdin);
    sscanf(entrada, "%d", &n);

    iris flor[n];
    iris flor_nova;

    leitura(n, flor);

    fgets(entrada, 100, stdin);
    sscanf(entrada, "%f %f %f %f",
           &flor_nova.cs,
           &flor_nova.ls,
           &flor_nova.cp,
           &flor_nova.lp);

    int tipo_desconhecido = classificar(flor_nova, n, flor);

    printf("%s", flor[tipo_desconhecido].tipo);

    return 0;
}