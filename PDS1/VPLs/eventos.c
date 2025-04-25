#include<stdio.h>
#include<stdlib.h>

enum meses {Janeiro, Fevereiro, Marco, Abril, Maio, Junho, Julho, Agosto, Setembro, Outubro, Novembro, Dezembro};

struct Data{
    int dia;
    enum meses m;
    int ano;
};

struct Evento{
    char nome[100];
    char local[100];
    struct Data d;
};

void cadastrar_eventos(struct Evento agenda[], int n){
    int i, m;
    if(n==0)
        printf("Nenhum evento encontrado!");

    for(i=0;i<n;i++){
        scanf("%s", agenda[i].nome);
        scanf("%s", agenda[i].local);
        scanf("%d", & agenda[i].d.dia);
        scanf("%d", & m);
        agenda[i].d.m = m;
        scanf("%d", & agenda[i].d.ano);
    }
}

void imprimir_eventos(struct Evento agenda[], struct Data d, int n){
    int m, i, aux;
    aux=0;
    i=1;
    struct Data x;
    
    scanf("%d", & x.dia);
    scanf("%d", & m);
    x.m = m;
    scanf("%d", & x.ano);
    
    for(i=0;i<n;i++){
        if(((agenda[i].d.dia) == x.dia) && ((agenda[i].d.m) == x.m) && ((agenda[i].d.ano) == x.ano)){
             printf("%s %s\n", agenda[i].nome, agenda[i].local);
             aux=1;
        }
        if(!aux){
            printf("Nenhum evento encontrado!");
        }
    }
}

int main()
{
    struct Evento agenda[400];
    int n;
    struct Data d;

    scanf("%d", & n);
    cadastrar_eventos(agenda, n);
    imprimir_eventos(agenda, d, n);
}
