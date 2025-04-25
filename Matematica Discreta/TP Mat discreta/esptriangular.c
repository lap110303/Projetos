#include <stdio.h>
#include <math.h>

int main (){
    int x, y, n, temp, lado, passos;
    float raiz;

    scanf("%d", &n);

    raiz = sqrt(n);

    if(raiz == floor(raiz)){
        temp = raiz;
        if(temp%2 == 0){
            x = -sqrt(n);
            y = -sqrt(n)/2;
        }
        else{
            x = sqrt(n);
            y = -sqrt(n)/2;
        }
    }
    else{
        lado = raiz;
        passos = n - (lado*lado);
        if(lado%2 == 0){
            x = ceil(-sqrt(n)) + passos;
            y = -sqrt(n)/2;
        }
        else{
            if(passos<=lado){
                x = floor(sqrt(n)) - passos;
                y = ceil(-sqrt(n)/2) + passos;
            }
            else{
                x = floor(sqrt(n)) - passos;
                y = (ceil(-sqrt(n)/2) + lado) - (passos - lado);
            }
        }
    }

    printf("Coordenadas em x e y: (%d, %d)\n", x, y);
}