#include<stdio.h>
#include<string.h>
struct cadastro{
    char nome[50];
    char marca[50];
    float preco;
};

int main()
{
    struct cadastro x[8];
    int i, j, prodmarca;
    int produtos[8]={0};
    char marc[50];
    float precototal;

    precototal=0;

    for(i=0;i<8;i++){
        gets(x[i].nome);
        gets(x[i].marca);
        scanf("%f", & x[i].preco);
        
        precototal+=x[i].preco;

        prodmarca=-1;

        for(j=0;j<8;j++){
            if(strcmp(marc[j],x[i].marca)==0){
                prodmarca=j;
                break;
            }
        }
        if(prodmarca==-1){
            prodmarca=strlen(marc);
            strcpy(marc[prodmarca],x[i].marca);
        }
        produtos[prodmarca]++;
    }
    for(i=0;i<strlen(marc);i++)
        printf("%s %d\n", marc[i], produtos[i]);
    
    printf("media total %.2f\n", precototal);

    for(i=0;i<strlen(marc);i++){
        float soma=0;
        int cont=0;
        for(j=0;j<8;j++){
            if (strcmp(marc[i], x[i].marca)==0){
                soma+=x[i].preco;
                cont++;
            }
        }
        printf("media %s %.2f\n", marc[i], soma/cont);
    }
}