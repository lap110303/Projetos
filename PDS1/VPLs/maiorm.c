#include<stdio.h>
int main()
{
    int i, j, x, y, maior;
    scanf("%d %d", &i, &j);
    
    int m[i][j];
    
    for(x=0;x<i;x++){
        for(y=0;y<j;y++)
            scanf("%d", &m[x][y]);
    }
    maior=m[0][0];
    
     for(x=0;x<i;x++){
        for(y=0;y<j;y++)
            if(m[x][y]>maior)
                maior=m[x][y];
    }
    printf("\n%d", maior);
}