#include <stdio.h>
#include <stdlib.h>

void ler(int *v, int n)
{
    char entrada[100];

    for (int i = 0; i < n; i++)
    {
        fgets(entrada, 100, stdin);
        sscanf(entrada, "%d", &v[i]);
    }
}

int main()
{

    char entrada[100];
    int *v;
    int n = 0;

    fgets(entrada, 100, stdin);
    sscanf(entrada, "%d", &n);
    v = (int *)malloc(n * sizeof(int));

    ler(v, n);

    for (int i = 0; i < n; i++)
    {
       printf("%d\n", v[i]);
    }
    
    free(v);

    return 0;
}