#include<stdio.h>
int main()
{
    int i, j, x, y;
    scanf("%d %d", &i, &j);
    int m[i][j];
    
    for(x=0;x<i;x++){
        for(y=0;y<j;y++)
            scanf("%d",& m[x][y]);
    }
    
    for(x=0;x<i;x++){
        for(y=0;y<j;y++)
            printf("%d ",(-1)*m[x][y]);
        printf("\n ");
    }
}