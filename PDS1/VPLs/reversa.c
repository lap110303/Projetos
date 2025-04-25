#include<stdio.h>
#include<string.h>
int main()
{
    char str1[100];
    char str2[100];
    int i, x;
    
    fgets(str1,100,stdin);
    x=strlen(str1)-1;
    
    for(i=0;i<=x;i++){
        str2[i]=str1[x-i];
    }
    printf("%s", str2);
}