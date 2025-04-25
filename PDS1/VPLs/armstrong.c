#include <stdio.h>
int main()
{
	int x, y, n, cont, ru, rd, rc, soma;
	float media;
	scanf("%d", & x);
	scanf("%d", & y);
	cont=0;
	soma=0;
	
	for(n=x; n<=y; n++){

		if(n/10 < 1){
			cont++;
			soma=soma+n;
		}
		
		else if (n/10>=1 && n/10<10){
			ru=n%10;
			rd=n%100-ru;
			rd=rd/10;
			
			if(rd*rd+ru*ru==n){
				cont++;
				soma=soma+n;
			}
		}
		else if (n/10>=100 && n/10<1000){
			ru=n%1;
			rd=n%100-ru;
			rc=n%1000-rd;
			rd=rd/10;
			rc=rc/100;
		}
			if (rc*rc*rc+rd*rd*rd+ru*ru*ru==n){
				cont++;
				soma=soma+n;
			}
	}
	media=soma/cont;
	printf("%d\n", cont);
	printf("%f\n", media);
	
	return 0;
}


