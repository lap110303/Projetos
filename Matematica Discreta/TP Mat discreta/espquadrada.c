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
            x = -sqrt(n)/2;
            y = -sqrt(n)/2;
        }
        else{
            x = sqrt(n)/2;
            y = (sqrt(n)/2)+1;
        }
    }
    else{
        lado = raiz;
        passos = n-(lado*lado);
        if(lado%2 == 0){
            if(passos <= lado){
                x = ceil((-sqrt(n)/2) + passos);
                y = -sqrt(n)/2;
            }
            else{
                x = sqrt(n)/2;
                y = ceil((-sqrt(n)/2)) + (passos-lado);
            }
        }
        else{
            if(passos<=lado){
                x = floor(sqrt(n)/2 - passos);
                y = (sqrt(n)/2)+1;
            }
            else{
                x = -sqrt(n)/2-1;
                y = floor(sqrt(n)/2+1) - (passos-lado);
            }
        }
    }

    printf("Coordenadas em x e y: (%d, %d)\n", x, y);
}