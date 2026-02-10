#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

// protótipos das funções fornecidas no arquivo passa_tempo.c
extern void inicia_tempo(void);
extern void passa_tempo(int tid, int gid, int pos, int decimos);

// ------------------- estruturas --------------------

typedef struct Waiter {
    int tid;
    int gid;
    struct Waiter *next;
    int paired;                // 0/1
    pthread_cond_t cond;       // condição específica deste waiter
} Waiter;

typedef struct Position {
    pthread_mutex_t lock;
    // fila de waiters (simples linked list)
    Waiter *head;
    Waiter *tail;
    int occupied; // número de ocupantes atualmente (0,1,2)
} Position;

typedef struct ThreadInfo {
    int tid;
    int gid;
    int start_delay; // em décimos
    int nsteps;
    int *positions; // array de posições (valores 1..N)
    int *times;     // tempos em décimos por posição
} ThreadInfo;

// ------------------- variáveis globais --------------------

Position *positions_arr = NULL; // indexed 1..N
int N_positions = 0;
int N_threads = 0;
ThreadInfo *threads_info = NULL;

// ------------------- utilitários da fila --------------------

static void pos_init(Position *p) {
    pthread_mutex_init(&p->lock, NULL);
    p->head = p->tail = NULL;
    p->occupied = 0;
}

static void enqueue_waiter(Position *p, Waiter *w) {
    w->next = NULL;
    if (p->tail == NULL) {
        p->head = p->tail = w;
    } else {
        p->tail->next = w;
        p->tail = w;
    }
}

static void remove_node(Position *p, Waiter *prev, Waiter *node) {
    if (prev == NULL) {
        p->head = node->next;
    } else {
        prev->next = node->next;
    }
    if (node == p->tail) {
        p->tail = prev;
    }
    node->next = NULL;
}

// tenta formar a primeira dupla possível na fila da posição p
// deve ser chamado com p->lock já preso
// retorna 1 se formou uma dupla (e sinalizou os dois waiters), 0 caso contrário
static int try_form_pair(Position *p) {
    if (p->occupied != 0) return 0; // só forma dupla quando posição está vazia
    Waiter *a_prev = NULL;
    for (Waiter *a = p->head; a != NULL; a_prev = a, a = a->next) {
        Waiter *b_prev = a;
        for (Waiter *b = a->next; b != NULL; b_prev = b, b = b->next) {
            if (a->gid != b->gid) {
                // pair found: remove b then a (remove later node first)
                remove_node(p, b_prev, b);
                remove_node(p, a_prev, a);
                // marcar pares
                a->paired = 1;
                b->paired = 1;
                // posição agora ocupada por 2
                p->occupied = 2;
                // sinalizar os dois waiters
                pthread_cond_signal(&a->cond);
                pthread_cond_signal(&b->cond);
                return 1;
            }
        }
    }
    return 0;
}

// ------------------- primitivas entra/sai --------------------

// A thread chama entra(pos_index, tid, gid) para entrar na posição pos_index.
// A chamada bloqueia até que a thread faça parte de uma dupla que entrará na posição.
// Implementação usa per-waiter condvar.
static void entra(int pos_index, int tid, int gid) {
    Position *p = &positions_arr[pos_index];
    Waiter w;
    w.tid = tid;
    w.gid = gid;
    w.next = NULL;
    w.paired = 0;
    pthread_cond_init(&w.cond, NULL);

    pthread_mutex_lock(&p->lock);

    // enfileira
    enqueue_waiter(p, &w);

    // se possível, tenta formar dupla agora (caso posição esteja vazia)
    try_form_pair(p);

    // espera até ser pareado
    while (!w.paired) {
        pthread_cond_wait(&w.cond, &p->lock);
    }
    // quando acorda, já faz parte de uma dupla e p->occupied foi ajustado
    pthread_mutex_unlock(&p->lock);

    pthread_cond_destroy(&w.cond);
}

// A thread libera sua ocupação em pos_index.
// Quando occupied chega a 0, tentamos formar dupla a partir da fila de espera.
static void sai(int pos_index) {
    Position *p = &positions_arr[pos_index];
    pthread_mutex_lock(&p->lock);

    if (p->occupied > 0) p->occupied--;
    else {
        // proteção: normalmente não deve acontecer
        p->occupied = 0;
    }

    if (p->occupied == 0) {
        // posição ficou vazia -> tentar formar uma nova dupla a partir da fila
        try_form_pair(p);
    }

    pthread_mutex_unlock(&p->lock);
}

// ------------------- thread routine --------------------

static void *thread_routine(void *arg) {
    ThreadInfo *ti = (ThreadInfo *)arg;
    // espera inicial (start_delay em décimos)
    struct timespec zzz;
    zzz.tv_sec = ti->start_delay / 10;
    zzz.tv_nsec = (ti->start_delay % 10) * 100L * 1000000L;
    nanosleep(&zzz, NULL);

    if (ti->nsteps <= 0) return NULL;

    // primeiro passo: entra na primeira posição
    int curpos = ti->positions[0];
    entra(curpos, ti->tid, ti->gid);
    passa_tempo(ti->tid, ti->gid, curpos, ti->times[0]);

    // passos seguintes
    for (int k = 1; k < ti->nsteps; ++k) {
        int nextpos = ti->positions[k];
        // tentar entrar no próximo (bloqueia até poder entrar em dupla)
        entra(nextpos, ti->tid, ti->gid);
        // só depois de ter entrado no próximo, libera a atual
        sai(curpos);
        // permanece no nextpos
        passa_tempo(ti->tid, ti->gid, nextpos, ti->times[k]);
        curpos = nextpos;
    }

    // ao terminar trajeto, libera a última posição e encerra
    sai(curpos);

    return NULL;
}

// ------------------- leitura e main --------------------

int main(void) {
    if (scanf("%d %d", &N_positions, &N_threads) != 2) {
        return 0;
    }
    if (N_positions < 1) return 0;
    if (N_threads < 0) return 0;

    // aloca estruturas
    positions_arr = calloc(N_positions + 1, sizeof(Position)); // index 1..N
    for (int i = 1; i <= N_positions; ++i) pos_init(&positions_arr[i]);

    threads_info = calloc(N_threads, sizeof(ThreadInfo));
    pthread_t *tids = calloc(N_threads, sizeof(pthread_t));

    // ler cada thread
    for (int i = 0; i < N_threads; ++i) {
        int tid, gid, start_delay, nsteps;
        if (scanf("%d %d %d %d", &tid, &gid, &start_delay, &nsteps) != 4) {
            // entrada inconsistente (mas enunciado garante formato correto)
            nsteps = 0;
        }
        threads_info[i].tid = tid;
        threads_info[i].gid = gid;
        threads_info[i].start_delay = start_delay;
        threads_info[i].nsteps = nsteps;
        threads_info[i].positions = NULL;
        threads_info[i].times = NULL;
        if (nsteps > 0) {
            threads_info[i].positions = malloc(sizeof(int) * nsteps);
            threads_info[i].times = malloc(sizeof(int) * nsteps);
            for (int j = 0; j < nsteps; ++j) {
                int p, t;
                if (scanf("%d %d", &p, &t) != 2) {
                    p = 1; t = 1;
                }
                threads_info[i].positions[j] = p; // pos in [1..N]
                threads_info[i].times[j] = t;
            }
        }
    }

    // inicia o tempo (conforme instrução)
    inicia_tempo();

    // cria threads
    for (int i = 0; i < N_threads; ++i) {
        pthread_create(&tids[i], NULL, thread_routine, &threads_info[i]);
    }

    // espera término
    for (int i = 0; i < N_threads; ++i) {
        pthread_join(tids[i], NULL);
    }

    // liberar memória
    for (int i = 0; i < N_threads; ++i) {
        free(threads_info[i].positions);
        free(threads_info[i].times);
    }
    free(threads_info);
    free(tids);
    free(positions_arr);

    return 0;
}
