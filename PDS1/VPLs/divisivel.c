#include <stdio.h>
#include <math.h>

int main() {
    int y;
    double x, z, soma, result;
    soma = 0;

    for(y = 1; y < 10000; y++) {
        z = y;
        x = 6.0 / (pow(3, z - (1.0 / 2.0)) * (2.0 * z - 1.0));
 // Convertendo 4 para double
        result = pow(-1.0, y - 1) * x;
        soma = soma + result;
    }
    printf("%f\n", soma);
    printf("%f", z);

    return 0;
}
