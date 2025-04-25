#include <stdio.h>
#include <string.h>

int main() {
    char string[100];
    int i, count = 0;

    printf("Digite uma string: ");
    fgets(string, 100, stdin);

    for(i =0; i <strlen(string); i++) {
        if(string[i] == 'a' || string[i] == 'A') {
            count++;
        }
    }

    printf("A letra 'a' aparece %d vezes na string.", count);

    return 0;
}