#include <stdio.h>
int bi(int ano){
	if(ano%4==0 && ano%100!=0 || ano%400==0)
		return 1;
	else
		return 0;
}

int main()
{   
	int x;
	while(scanf("%d", &x) !=EOF)
	    printf("%d\n", bi(x));
}