#include<stdio.h>
int main()
{
    int x2, m2, x3, m3, x5, m5, x30, m30, N;
    scanf("%d", & N);
    
    x2=2;
    m2=0;
    while (x2<=N){
        m2++;
        x2+=2;
    }
    
    x3=3;
    m3=0;
    while (x3<=N){
        m3++;
        x3+=3;
    }
    
    x5=5;
    m5=0;
    while (x5<=N){
        m5++;
        x5+=5;
    }
    
    x30=30;
    m30=0;
    while(x30<=N){
        m30++;
        x30+=30;
    }
    
    printf("Múltiplos de 2: %d\n", m2);
    printf("Múltiplos de 3: %d\n", m3);
    printf("Múltiplos de 5: %d\n", m5);
    printf("Múltiplos de todos: %d\n", m30);
}