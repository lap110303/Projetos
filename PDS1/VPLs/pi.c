#include <stdio.h>
#include <math.h>
int main()
{
    double x;
    scanf("%lf", &x);
    double n=4.0, y=3.0;
    int t=1;
    while (n>M_PI+x || n<M_PI-x){
        if (t%2==0){
            n+=4.0/y;
        }
        else{
            n-=4.0/y;
        }
        t++;
        y+=2;
    }
    printf("%d\n", t);
}