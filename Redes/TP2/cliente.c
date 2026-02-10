#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include "common.h"

typedef struct {
    int id;
    char nome[100];
    int status_monitorado;
} CidadeLocal;

typedef struct {
    int ativa;
    int id_cidade;
    int id_equipe;
} MissaoDrone;

CidadeLocal cidades[NUM_CIDADES];
MissaoDrone fila_missoes[10];
int fim_fila = 0;
int inicio_fila = 0;

pthread_mutex_t mutex_dados = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_missao = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_missao = PTHREAD_COND_INITIALIZER;

int sockfd;
struct sockaddr_in serv_addr;
socklen_t addr_len = sizeof(serv_addr);

void* thread_monitoramento(void* arg);
void* thread_telemetria(void* arg);
void* thread_recepcao(void* arg);
void* thread_drone(void* arg);

void carregar_cidades(const char* filename);
void enviar_ack(int tipo_ack);

int main(int argc, char *argv[]) {
    srand(time(NULL));

    carregar_cidades("grafo_amazonia_legal.txt");

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Endereço inválido");
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor 127.0.0.1:%d\n", SERVER_PORT);

    pthread_t t1, t2, t3, t4;

    pthread_create(&t1, NULL, thread_monitoramento, NULL);
    pthread_create(&t2, NULL, thread_telemetria, NULL);
    pthread_create(&t3, NULL, thread_recepcao, NULL);
    pthread_create(&t4, NULL, thread_drone, NULL);

    printf("Iniciando threads...\n");
    printf("[Thread Monitoramento] Iniciada\n");
    printf("[Thread Telemetria] Iniciada\n");
    printf("[Thread Recepcao] Iniciada\n");
    printf("[Thread Drone] Iniciada\n");

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);
    pthread_join(t4, NULL);

    close(sockfd);
    return 0;
}

void* thread_monitoramento(void* arg) {
    while(1) {
        pthread_mutex_lock(&mutex_dados);
        for(int i=0; i<NUM_CIDADES; i++) {
            int r = rand() % 100;
            if (r < PROB_ALERTA) {
                cidades[i].status_monitorado = STATUS_ALERTA;
            } else {
                cidades[i].status_monitorado = STATUS_OK;
            }
        }
        pthread_mutex_unlock(&mutex_dados);
        
        sleep(5);
    }
    return NULL;
}

void* thread_telemetria(void* arg) {
    while(1) {
        header_t head;
        head.tipo = MSG_TELEMETRIA;
        head.tamanho = sizeof(payload_telemetria_t);

        payload_telemetria_t body;
        body.total = NUM_CIDADES;

        printf("\n[ENVIANDO TELEMETRIA]\n");
        printf("Total de cidades: %d\n", NUM_CIDADES);

        pthread_mutex_lock(&mutex_dados);
        for(int i=0; i<NUM_CIDADES; i++) {
            body.dados[i].id_cidade = cidades[i].id;
            body.dados[i].status = cidades[i].status_monitorado;
            
            if(cidades[i].status_monitorado == STATUS_ALERTA) {
                printf("ALERTA: %s (ID=%d)\n", cidades[i].nome, cidades[i].id);
            }
        }
        pthread_mutex_unlock(&mutex_dados);

        char buffer[sizeof(header_t) + sizeof(payload_telemetria_t)];
        memcpy(buffer, &head, sizeof(header_t));
        memcpy(buffer + sizeof(header_t), &body, sizeof(payload_telemetria_t));

        sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
        printf("-> Telemetria enviada. Aguardando ACK...\n");

        sleep(INTERVALO_TELEMETRIA);
    }
    return NULL;
}

void* thread_recepcao(void* arg) {
    char buffer[BUFFER_SIZE];
    socklen_t len;

    while(1) {
        len = sizeof(serv_addr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &serv_addr, &len);
        
        if (n < sizeof(header_t)) continue;

        header_t* head = (header_t*) buffer;
        void* payload = buffer + sizeof(header_t);

        if (head->tipo == MSG_ACK) {
            payload_ack_t* ack = (payload_ack_t*) payload;
            printf(". ACK recebido do servidor (Tipo: %d)\n", ack->status);
        }
        else if (head->tipo == MSG_EQUIPE_DRONE) {
            payload_equipe_drone_t* ordem = (payload_equipe_drone_t*) payload;
            printf("\n[ORDEM DE DRONE RECEBIDA]\n");
            printf("Cidade: %s (ID=%d)\n", cidades[ordem->id_cidade].nome, ordem->id_cidade);
            printf("Equipe: %s (ID=%d)\n", cidades[ordem->id_equipe].nome, ordem->id_equipe);

            enviar_ack(1);

            pthread_mutex_lock(&mutex_missao);
            if (fim_fila < 10) {
                fila_missoes[fim_fila].ativa = 1;
                fila_missoes[fim_fila].id_cidade = ordem->id_cidade;
                fila_missoes[fim_fila].id_equipe = ordem->id_equipe;
                fim_fila++;
                printf("-> Missao registrada para execucao\n");
                pthread_cond_signal(&cond_missao);
            } else {
                printf("-> ERRO: Fila de missoes cheia!\n");
            }
            pthread_mutex_unlock(&mutex_missao);
        }
    }
    return NULL;
}

void* thread_drone(void* arg) {
    while(1) {
        MissaoDrone missao_atual;

        pthread_mutex_lock(&mutex_missao);
        while (inicio_fila == fim_fila) {
            pthread_cond_wait(&cond_missao, &mutex_missao);
        }
        missao_atual = fila_missoes[inicio_fila];
        inicio_fila++;
        if(inicio_fila == fim_fila) { inicio_fila = 0; fim_fila = 0; }
        pthread_mutex_unlock(&mutex_missao);

        printf("\n[MISSAO EM ANDAMENTO]\n");
        printf("Equipe %s atuando em %s\n", cidades[missao_atual.id_equipe].nome, cidades[missao_atual.id_cidade].nome);

        int tempo = (rand() % 30) + 1;
        printf(". Tempo estimado: %d segundos\n", tempo);
        sleep(tempo);

        printf(". Missao concluida!\n");

        header_t head;
        head.tipo = MSG_CONCLUSAO;
        head.tamanho = sizeof(payload_conclusao_t);

        payload_conclusao_t body;
        body.id_cidade = missao_atual.id_cidade;
        body.id_equipe = missao_atual.id_equipe;

        char buffer[sizeof(header_t) + sizeof(payload_conclusao_t)];
        memcpy(buffer, &head, sizeof(header_t));
        memcpy(buffer + sizeof(header_t), &body, sizeof(payload_conclusao_t));

        sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
        printf("-> Conclusao enviada ao servidor\n");
    }
    return NULL;
}

void enviar_ack(int tipo_ack) {
    header_t head;
    head.tipo = MSG_ACK;
    head.tamanho = sizeof(payload_ack_t);
    payload_ack_t body;
    body.status = tipo_ack;

    char buffer[sizeof(header_t) + sizeof(payload_ack_t)];
    memcpy(buffer, &head, sizeof(header_t));
    memcpy(buffer + sizeof(header_t), &body, sizeof(payload_ack_t));

    sendto(sockfd, buffer, sizeof(buffer), 0, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
}

void carregar_cidades(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }
    int n, m;
    fscanf(f, "%d %d", &n, &m);
    for(int i=0; i<n; i++) {
        int id;
        fscanf(f, "%d", &id);
        
        char temp_line[256];
        fgets(temp_line, sizeof(temp_line), f);

        int len = strlen(temp_line);
        int p = len - 1;
        while(p >= 0 && (temp_line[p] == '\n' || temp_line[p] == ' ' || temp_line[p] >= '0' && temp_line[p] <= '9')) {
             temp_line[p] = '\0';
             p--;
        }
        
        char* start = temp_line;
        while(*start == ' ') start++;

        cidades[id].id = id;
        strcpy(cidades[id].nome, start);
        cidades[id].status_monitorado = STATUS_OK;
    }
    fclose(f);
}