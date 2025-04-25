#include<stdio.h>
int main()
{
    int x, y;
    int fibonacci[800];
    fibonacci[0]=0;
    fibonacci[1]=1;
    x=0;
    
    for(x>=2; x<=800; x++)
        fibonacci[x]=fibonacci[x-1]+fibonacci[x-2];
    
    while(y>=0 && y<=800){
        scanf("%d\n", &y);
        
        if(y>=0 && y<=800)
        printf("%d\n", fibonacci[y]);
    }
}