#include <stdio.h>

long long int primo(long long int p){
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

int main() 
{
	long long int y;
	while(scanf("%lld", &y) !=EOF)
	    printf("%lld\n", primo(y));
}