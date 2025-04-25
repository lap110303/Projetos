#include<stdio.h>
long long primo(long long int p){
    int j, cont;
    long long int i;
    cont=0;
	if(p<2){
		j=-1;
		return j;
	}
	else{
		for(i=1;i<=p;i++){
        		if(p%i==0)
            	cont++;
        	}
        if(cont==2){
        	j=1;
        	return j;
        }
		else{
			j=0;
			return j;
		}
    }
}

long long int somaprimo(long long int p){
    int cont;
    long long int soma;
    while (cont<p){
        if(primo(p)){
            soma+=p;
            cont++;
        }
    }
    return soma;
}

int main()
{
    long long int nmr;
    while(scanf("%d", & nmr)!=EOF);
        printf("%lld\n", somaprimo(nmr));
}