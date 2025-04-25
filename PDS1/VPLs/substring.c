#include<stdio.h>
#include<string.h>
int main()
{
    char str1[100];
    char str2[100];
    int x, y, condicao;
    
    fgets(str1,100,stdin);
    fgets(str2,100,stdin);
    
    if((strlen(str1)-1)==0){
        printf("Não é substring");
        return 0;
    }
    
    for(x=0;x<=strlen(str1)-strlen(str2);x++){
       condicao=1;
       for(y=0;y<strlen(str2);y++){
           if(str1[x+y]!=str2[y]){
               condicao=0;
               break;
            }
        }
   
        if(condicao){
            printf("É substring");
            break;
        }
    }

    
    
    if(!condicao)
        printf("Não é substring");
}