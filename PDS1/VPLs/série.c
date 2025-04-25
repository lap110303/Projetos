#include <stdio.h>
int main()
{
  int n, x0, x1, xn, x;
  scanf("%d", & n);
  scanf("%d", & x0);
  scanf("%d", & x1);
  printf("x0 = %d\n", x0);
  printf("x1 = %d\n", x1);
  
  for (x = 2; x <= n; x++){
    xn = 4*x1-2*x0;
    printf("x%d = %d\n", x, xn);
    x0=x1;
    x1=xn;
  }
}