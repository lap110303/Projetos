#include<stdio.h>
#include<string.h>
int main()
{
    char str1[100];
    char str2[100];
    char str3[100];
    int x, y;
    x=0;
    y=0;
    
    setbuf(stdin, NULL);
    gets(str1);
    gets(str2);
    
    while(str1[x]!='\0'){
        str3[y]=str1[x];
        x++;
        y++;
    }
    x=0;
    
    while(str2[x]!='\0'){
        str3[y]=str2[x];
        x++;
        y++;
    }
    
    printf("%s",str3);
}