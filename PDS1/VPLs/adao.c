#include <stdio.h>
#include <stdbool.h>

int main(){
    int x = 0;
    int soma = 0;

    scanf("%i", &x);

    for (int i = 2; i < x; i++){
        bool primo = true; // reinicializa a cada número

        for (int j = 2; j < i; j++){
            if (i % j == 0){
                primo = false;
                break; // já sabemos que não é primo
            }
        }

        if (primo){
            soma += i;
        }
    }

    printf("Soma dos primos menores que %d: %d\n", x, soma);

    return 0;
}
