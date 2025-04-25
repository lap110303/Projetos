#include<stdio.h>
#include<string.h>
struct registro{
    char nome[50];
    int idade;
    float salario;
};

int main()
{
    struct registro x[4];
    int i;

    for(i=0;i<4;i++){
        gets(x[i].nome);
        scanf("%d", & x[i].idade);
        scanf("%f", & x[i].salario);
    }
}