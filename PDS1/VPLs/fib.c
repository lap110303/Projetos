#include <stdio.h>
#include <stdlib.h>

long int fibo(int n)
{
    int f;
    if (n > 1)
        f = fibo(n - 1) + fibo(n - 2);
    else if (n==1)
        f = 1;
    else
        f = 0;
    return f;
}

int main()
{
    
    char entrada[50];
    int num = 0;

    fgets(entrada, 50, stdin);
    sscanf(entrada, "%d", &num);

    long int fibonacci = fibo(num);
    printf("%ld", fibonacci);

    return 0;
}