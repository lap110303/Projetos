#include<stdio.h>
#include<math.h>

void media(double v[], int n, int *p){

    double x = 0;
    double diferenca = 0;

    for(int j = 0; j < n; j++){
        x += v[j];
    }
    x = x/n;
    diferenca = fabs(v[0] - x);
    for (int j = 0; j < n; j++){
        if ( fabs( v[j] - x ) < diferenca ){
            diferenca = fabs(v[j] - x);
            *p = j;
        }
    }

}

int main (int argc, char **argv) {

    int *p;
    p = &argv[1];
    int n = 0;

    scanf("%d", &n);
    double v[n];

    for(int i = 0; i < n; i++){
        scanf("%le", &v[i]);
    }

    media(v,n,p);
    printf("%d\n", *p);

}