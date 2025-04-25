//---------------------------------------------------------------------
// Arquivo      : evalenum.c
// Conteudo     : avaliacao de desempenho de algoritmos de ordenacao 
// Autor        : Wagner Meira Jr. (meira@dcc.ufmg.br)
// Historico    : 2023-11-04 - arquivo criado
//---------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define ALGINSERTION 1
#define ALGSELECTION 2
#define ALGQSORT     3
#define ALGQSORT3    4
#define ALGQSORTINS  5
#define ALGQSORT3INS 6
#define ALGSHELLSORT 7
#define ALGRECSEL    8

typedef struct alg{
  int num;
  char * name;
} alg_t;

alg_t algvet[]={
  {ALGINSERTION,"i"},
  {ALGSELECTION,"s"},
  {ALGQSORT,"q"},
  {ALGQSORT3,"q3"},
  {ALGQSORTINS,"qi"},
  {ALGQSORT3INS,"q3i"},
  {ALGSHELLSORT,"h"},
  {ALGRECSEL,"rs"},
  {0,0}
};

int name2num(char * name){
  int i=0;
  while (algvet[i].num){
    if (!strcmp(algvet[i].name,name)) return algvet[i].num;
    i++;
  }
  return 0;
}

char * num2name(int num){
  int i=0;
  while (algvet[i].num){
    if (algvet[i].num==num) return algvet[i].name;
    i++;
  }
  return 0;
}

typedef struct opt{
  int size;
  int seed;
  int alg;
} opt_t;

typedef struct sortperf{
  int cmp;
  int move;
  int calls;
} sortperf_t;

void resetcounter(sortperf_t * s){
// Descricao: inicializa estrutura
// Entrada: 
// Saida: s
  s->cmp = 0;
  s->move = 0;
  s->calls = 0;
}

void inccmp(sortperf_t * s, int num){
// Descricao: incrementa o numero de comparacoes em num 
// Entrada: s, num
// Saida: s
  s->cmp += num;
}

void incmove(sortperf_t * s, int num){
// Descricao: incrementa o numero de movimentacoes de dados em num 
// Entrada: s, num
// Saida: s
  s->move += num;
}

void inccalls(sortperf_t * s, int num){
// Descricao: incrementa o numero de chamadas de função em num 
// Entrada: s, num
// Saida: s
  s->calls += num;
}

char * printsortperf(sortperf_t * s, char * str, char * pref){
// Descricao: gera string com valores de sortperf 
// Entrada: s, pref
// Saida: str

 sprintf(str,"%s cmp %d move %d calls %d", pref, s->cmp, s->move, s->calls); 
 return str;
}


void initVector(int * vet, int size){
// Descricao: inicializa vet com valores aleatorios
// Entrada: vet
// Saida: vet
  int i;
  // inicializa a parte alocada da vetor com valores aleatorios
  for (i=0; i<size; i++){
    vet[i] = (int)(drand48()*size);
  }
}

void printVector(int * vet, int size){
// Descricao: inicializa vet com valores aleatorios
// Entrada: vet
// Saida: vet
  int i;
  for (i=0; i<size; i++){
    printf("%d ",vet[i]);
  }
  printf("\n");
}

void swap(int *xp, int *yp, sortperf_t *s){
    int temp = *xp;
    *xp = *yp;
    *yp = temp;
    incmove(s,3);
}

// shellsort
void shellSort(int *A, int n, sortperf_t *s) {
    int gap, i, j, temp;
    inccalls(s, 1);
    // Define o espaçamento inicial como metade do tamanho do array
    for (gap = n / 2; gap > 0; gap /= 2) {
        // Percorre o array com o espaçamento definido por 'gap'
        for (i = gap; i < n; i++) {
            // Armazena o elemento atual em 'temp'
            temp = A[i];
            incmove(s,2);
            // Move os elementos que são maiores que 'temp' para frente do seu espaço
            for (j = i; j >= gap && A[j - gap] > temp; j -= gap) {
                A[j] = A[j - gap];
                inccmp(s, 1);
                incmove(s, 1);
            }
            // Insere 'temp' na posição correta
            A[j] = temp;
            inccmp(s, 1);
        }
    }
}


// recursive selection sort
void recursiveSelectionSort(int arr[], int l, int r, sortperf_t * s)
{
    inccalls(s, 1);
    // find the minimum element in the unsorted subarray `[i…n-1]`
    // and swap it with `arr[i]`
    int min = l;
    for (int j = l + 1; j <= r; j++)
    {
        // if `arr[j]` is less, then it is the new minimum
        inccmp(s,1);
        if (arr[j] < arr[min]) {
            min = j;    // update the index of minimum element
        }
    }

    // swap the minimum element in subarray `arr[i…n-1]` with `arr[i]`
    if (min != l)
      swap(&arr[min], &arr[l], s);

    if (l + 1 < r) {
        recursiveSelectionSort(arr, l + 1, r, s);
    }
}

// selection sort
void selectionSort(int arr[], int l, int r, sortperf_t *s) {
    inccalls(s, 1);
    int i, j, min_idx;

    for (i = l; i < r; i++) {
        min_idx = i;
        for (j = i + 1; j <= r; j++) {
            // Incrementa o número de comparações
            inccmp(s, 1);
            if (arr[j] < arr[min_idx])
                min_idx = j;
        }
        // Verifica se o índice do elemento mínimo é diferente do índice atual antes de trocar
        if (min_idx != i) {
            // Troca os elementos apenas se forem diferentes
            swap(&arr[min_idx], &arr[i], s);
        }
    }
}



//insertion sort
void insertionSort(int v[], int l, int r, sortperf_t *s) {
    inccalls(s, 1);
    int i, j, aux;
    for (i = l + 1; i <= r; i++) {
        aux = v[i];
        j = i - 1;
        incmove(s, 2);

        // Move os elementos maiores que key para uma posição à frente de sua posição atual
        while (j >= l && v[j] > aux) {
            v[j + 1] = v[j];
            inccmp(s, 1);
            incmove(s, 1);
            j = j - 1;
        }

        v[j + 1] = aux;
        inccmp(s, 1);
    }
}



// median of 3 integers
int median (int a, int b, int c) {
    if ((a <= b) && (b <= c)) return b;  // a b c
    if ((a <= c) && (c <= b)) return c;  // a c b
    if ((b <= a) && (a <= c)) return a;  // b a c
    if ((b <= c) && (c <= a)) return c;  // b c a
    if ((c <= a) && (a <= b)) return a;  // c a b
    return b;                            // c b a
}

// quicksort partition using median of 3
void partition3(int *A, int l, int r, int *i, int *j, sortperf_t *s) {
    inccalls(s, 1);
    int mid = (r + l) / 2;
    int pivot = median(A[l], A[mid], A[r]);
    *i = l;
    *j = r;

    while (*i <= *j) {
        while (A[*i] < pivot) {
            (*i)++;
            inccmp(s, 1);
        }
        while (A[*j] > pivot) {
            (*j)--;
            inccmp(s, 1);
        }
        inccmp(s, 1);
        if (*i <= *j) {
            swap(&A[*i], &A[*j], s);
            (*i)++;
            (*j)--;
        }
        inccmp(s, 1);
    }
}



// standard quicksort partition
void partition(int *A, int l, int r, int *i, int *j, sortperf_t *s) {
    int pivot = A[(l + r)/2];
    *i = l;
    *j = r;
    while (*i <= *j) {
        while (A[*i] < pivot) {
            (*i)++;
            inccmp(s, 1);
        }
        while (A[*j] > pivot) {
            (*j)--;
            inccmp(s, 1);
        }
        inccmp(s, 1);
        if (*i <= *j) {
            swap(&A[*i], &A[*j], s);
            (*i)++;
            (*j)--;
        }
        inccmp(s, 1);
    }
}

// quicksort
void quickSort(int * A, int l, int r, sortperf_t *s) {
  inccalls(s, 2);
  int j, i;
  j = 0;
  i = 0;
  partition(A, l, r, &i, &j, s);
  if(l < j){
    quickSort(A, l, j, s);
  }
  if(i < r){
    quickSort(A, i, r, s);
  }
}

// quicksort with median of 3
void quickSort3(int * A, int l, int r, sortperf_t *s) {
  inccalls(s, 1);
  int j, i;
  j = 0;
  i = 0;
  partition3(A, l, r, &i, &j, s);
  if(l < j){
    quickSort3(A, l, j, s);
  }
  if(i < r){
    quickSort3(A, i, r, s);
  }
}

// quicksort with insertion for small partitions
void quickSortIns(int *A, int l, int r, sortperf_t *s) {
    int i, j;
    i = 0;
    j = 0;

    partition(A, l, r, &i, &j, s);

    if (j - l > 50) { // Using insertion sort for small partitions
        quickSortIns(A, l, j, s);
    } else if(l < j){
        insertionSort(A, l, j, s);
    }
    if (r - i > 50){
      quickSortIns(A, i, r, s);
    } else if(i < r){
      insertionSort(A, i, r, s);
    }
    inccalls(s, 2);
}

// quicksort with insertion for small partitions and median of 3
void quickSort3Ins(int *A, int l, int r, sortperf_t *s) {
    int i, j;
    i = 0;
    j = 0;

    partition3(A, l, r, &i, &j, s);

    if (j - l > 50) { // Using insertion sort for small partitions
        quickSort3Ins(A, l, j, s);
    } else if(l < j){
        insertionSort(A, l, j, s);
    }
    if (r - i > 50){
      quickSort3Ins(A, i, r, s);
    } else if(i < r){
      insertionSort(A, i, r, s);
    }
    inccalls(s, 1);
}

void uso()
// Descricao: imprime as opcoes de uso
// Entrada: nao tem
// Saida: impressao das opcoes de linha de comando
{
  fprintf(stderr,"sortperf\n");
  fprintf(stderr,"\t-z <int>\t(vector size)\n");
  fprintf(stderr,"\t-s <int>\t(initialization seed)\n");
  fprintf(stderr,"\t-a <s|i|q|q3|qi|q3i|h|rs>\t(algorithm)\n");
  fprintf(stderr,"\t    s\tselection\n");
  fprintf(stderr,"\t    i\tinsertion\n");
  fprintf(stderr,"\t    q\tquicksort\n");
  fprintf(stderr,"\t    q3\tquicksort+median3\n");
  fprintf(stderr,"\t    qi\tquicksort+insertion\n");
  fprintf(stderr,"\t    q3i\tquicksort+median3+insertion\n");
  fprintf(stderr,"\t    h\theapsort\n");
  fprintf(stderr,"\t    rs\trecursive selection\n");
}

void parse_args(int argc, char ** argv, opt_t * opt)
// Descricao: le as opcoes da linha de comando e inicializa variaveis
// Entrada: argc, argv, opt
// Saida: opt
{
     // variaveis externas do getopt
     extern char * optarg;
     extern int optind;

     // variavel auxiliar
     int c;

     // inicializacao variaveis globais para opcoes
     opt->seed = 1;
     opt->size = 10;
     opt->alg = 1;

     // getopt - letra indica a opcao, : junto a letra indica parametro
     // no caso de escolher mais de uma operacao, vale a ultima
     while ((c = getopt(argc, argv, "z:s:a:h")) != EOF){
       switch(c) {
         case 'z':
	          opt->size = atoi(optarg);
                  break;
         case 's':
	          opt->seed = atoi(optarg);
                  break;
         case 'a':
		  opt->alg = name2num(optarg);
                  break;
         case 'h':
         default:
                  uso();
                  exit(1);

       }
     }
     if (!opt->alg) {
       uso();
       exit(1);
     }
}

void clkDiff(struct timespec t1, struct timespec t2,
                   struct timespec * res)
// Descricao: calcula a diferenca entre t2 e t1, que e armazenada em res
// Entrada: t1, t2
// Saida: res
{
  if (t2.tv_nsec < t1.tv_nsec){
    // ajuste necessario, utilizando um segundo de tv_sec
    res-> tv_nsec = 1000000000+t2.tv_nsec-t1.tv_nsec;
    res-> tv_sec = t2.tv_sec-t1.tv_sec-1;
  } else {
    // nao e necessario ajuste
    res-> tv_nsec = t2.tv_nsec-t1.tv_nsec;
    res-> tv_sec = t2.tv_sec-t1.tv_sec;
  }
}


int main (int argc, char ** argv){
  sortperf_t s;
  int * vet;
  char buf[200];
  char pref[100];
  opt_t opt;
  struct timespec inittp, endtp, restp;
  int retp;

  // parse_args
  parse_args(argc,argv,&opt);

  // malloc with opt.size+1 for heapsort
  vet = (int *) malloc((opt.size+1)*sizeof(int));

  // initialize
  resetcounter(&s);
  srand48(opt.seed);
  initVector(vet, opt.size);
  vet[opt.size] = vet[0]; // for heapsort
  //if (opt.size < 100) printVector(vet, opt.size);

  retp = clock_gettime(CLOCK_MONOTONIC, &inittp);
  
  // execute algorithm
  switch (opt.alg){
    case ALGINSERTION:
         insertionSort(vet, 0, opt.size-1, &s);
         break;
    case ALGSELECTION:
         selectionSort(vet, 0, opt.size-1, &s);
         break;
    case ALGQSORT:
         quickSort(vet, 0, opt.size-1, &s);
         break;
    case ALGQSORT3:
         quickSort3(vet, 0, opt.size-1, &s);
         break;
    case ALGQSORTINS:
         quickSortIns(vet, 0, opt.size-1, &s);
         break;
    case ALGQSORT3INS:
         quickSort3Ins(vet, 0, opt.size-1, &s);
         break;
    case ALGSHELLSORT:
         shellSort(vet, opt.size, &s);
         break;
    case ALGRECSEL:
         recursiveSelectionSort(vet, 0, opt.size-1, &s);
         break;
  }
  retp = clock_gettime(CLOCK_MONOTONIC, &endtp);
  clkDiff(inittp, endtp, &restp);


  //if (opt.size<100) printVector(vet, opt.size);

  // print stats
  sprintf(pref,"alg %s seed %d size %d time %ld.%.9ld",
          num2name(opt.alg), opt.seed,opt.size,restp.tv_sec,restp.tv_nsec);

  printsortperf(&s,buf,pref);
  printf("%s\n",buf);

  exit(0);
}