#include<stdio.h>

int diapassado(int dia, int mes, int ano){
    int meses[]={31,28,31,30,31,30,31,31,30,31,30,31};
    int diames=0;
    int diadata;
    
    diadata=(2020-ano-1)*365+(2020-ano-1)/4;
    
    for(int i=0;i<mes-1;i++)
        diames+=meses[i];
    
    diadata+=diames+dia;
    
    if(ano%4==0 && (ano%100!=0 || ano%400==0) && mes>2)
        diadata++;
    
    return diadata;
}
int main()
{
    int dia, mes, ano;
    int diasatedata;
    while(scanf("%d %d %d", &dia, &mes, &ano)!=EOF){
        diasatedata=diapassado(dia, mes, ano);
        printf("%d\n", diasatedata);
    }
}