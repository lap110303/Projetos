#include<stdio.h>
int main()
{
    int i[10];
    int j[10];
    int x;
    
    for(x=0;x<10;x++)
        scanf("%d", &i[x]);
        
    for(x=0;x<10;x++){
        j[x]=i[9-x];
        printf("%d\n", j[x]);
    }
    
    
}