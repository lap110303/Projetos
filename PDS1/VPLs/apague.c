#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* apagar(char s[], char c)
{
    
    if (s[0] == '\0')
        return &s[0];
    if(s[0] == c)
    {
        char l = '\0';
        for (int i = 0; s[i] != l; i++)
        { 
            s[i] = s[i+1];
        }
    }
    
    apagar(&(s[1]), c);
    if(s[0] == c)
    {
        char l = '\0';
        for (int i = 0; s[i] != l; i++)
        { 
            s[i] = s[i+1];
        }
    }
    apagar(&(s[1]), c);
    return &s[0];
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

    char* palavra = apagar(s, c);
    printf("%s", palavra);

    return 0;
}