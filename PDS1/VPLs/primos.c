#include<stdio.h>
int main()
{
    int x, y, cont,i;
    scanf("%d", &x);
    scanf("%d", &y);
    
    if(x<0 && y>10000)
    return 0;
    
    while(x<=y){
    cont=0;
    
        for(i=1;i<=x;i++){
        if(x%i==0)
            cont++;
        }
        if(cont==2){
        printf("\n%d", x);
        }
    x++;
    }
}