#include<stdio.h>
#include<string.h>
struct endereco{
    char rua[50];
    int numero;
    char estado[50];
}

struct registro{
    char nome[50];
    int idade;
    float salario;
    struct endereco end;
};