#include<stdio.h>

int main()
{
    float f, c;
    printf("Digite a temperatura em Celsius\n");
    scanf("%f", &c);
    f = c*1.8+32;
    printf("Temperatura em Fahrenheit = %.0f", f);
}