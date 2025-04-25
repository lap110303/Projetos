#include<stdio.h>
#include<string.h>
#include<math.h>

void primos (unsigned long long int m, int *p1, int *p2){

    int aux = 1, aux1 = 1, aux2 = 1, aux3 = 1;

    for (unsigned long long int i = 2; aux1; i++){
        aux = 1;
        for(unsigned long long int j = 2; j < sqrt(i) + 1 && aux; j++) {
            if(i % j == 0){
                aux = 0;
            }
        }
        if (aux){
            if (i < m) *p1 = i;
            else aux1 = 0;
        }
    }
    for (unsigned long long int i = 2; aux3; i++){
            aux2 = 1;
            for(unsigned long long int j = 2; j < sqrt(i) + 1 && aux2; j++) {
                if(i % j == 0){
                aux2 = 0;
            }
        }
        if (aux2 && i > m) {
        *p2 = i;
        aux3 = 0;
        }
    }
}

int main(int argc, char **argv)
{

    unsigned long long int num = 0;
    int *p1,*p2;
    p1 = &argv[1];
    p2 = &argv[2];

    scanf("%lld\n", &num);
    primos(num,p1,p2);
    printf("%d\n%d\n", *p1,*p2);

}