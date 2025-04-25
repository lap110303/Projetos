#include <stdio.h>
int main()
{
    int x;
    x=233;
    do{
        printf("%d\n", x);
        
        if(x>=300 && x<=400)
            x+=3;
        else
            x+=5;
    }
        while(x<=457);
}