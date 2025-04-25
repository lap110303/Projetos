#include<stdio.h>
#include<string.h>
int main()
{
    char str[100];
    char c1='c';
    char c2='s';
    int i;
    fgets(str,100,stdin);
    scanf("%c %c", &c1, &c2);
    
    for(i=0;i<strlen(str);i++){
        if(str[i]==c1){
            str[i]=c2;
            break;
        }
    }
    printf("%s",str);
}