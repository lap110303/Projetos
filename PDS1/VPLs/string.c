#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int contador(char s[], char c)
{
    if (s[0] == '\0')
        return 0;
    return (c == s[0]) + contador(&(s[1]), c);
}

int main()
{
    char entrada[50];
    char s[50];
    char c;

    fgets(entrada, 50, stdin);
    sscanf(entrada, "%s", s);

    fgets(entrada, 50, stdin);
    sscanf(entrada, "%c", &c);

    int num = contador(s, c);
    printf("%d", num);

    return 0;
}