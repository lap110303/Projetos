#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <limits.h>
#include <stdbool.h>
#include "common.h"

#define INFINITO INT_MAX

typedef struct {
    int id;
    char nome[100];
    int tipo;
    int status_atual;
    int missao_ativa;
} Cidade;

typedef struct {
    int destino;
    int peso;
    struct Aresta* prox;
} Aresta;

Cidade cidades[NUM_CIDADES];
int adj[NUM_CIDADES][NUM_CIDADES];
int num_cidades, num_arestas;
int equipe_ocupada[NUM_CIDADES];

void carregar_grafo(const char* filename);
void processar_telemetria(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, payload_telemetria_t* payload);
void processar_conclusao(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, payload_conclusao_t* payload);
void enviar_ack(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, int tipo_ack);
void despachar_drone(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, int id_cidade_alerta);
int executar_dijkstra(int origem, int* id_melhor_equipe);

int main(int argc, char *argv[]) {
    carregar_grafo("grafo_amazonia_legal.txt");
    memset(equipe_ocupada, 0, sizeof(equipe_ocupada));

    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro ao abrir socket");
        exit(EXIT_FAILURE);
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erro no bind");
        exit(EXIT_FAILURE);
    }

    printf("Servidor escutando na porta %d...\n", SERVER_PORT);

    char buffer[BUFFER_SIZE];
    socklen_t len;
    
    while (1) {
        len = sizeof(cli_addr);
        int n = recvfrom(sockfd, buffer, BUFFER_SIZE, MSG_WAITALL, (struct sockaddr *) &cli_addr, &len);
        
        if (n < sizeof(header_t)) continue;

        header_t* header = (header_t*) buffer;
        void* payload = buffer + sizeof(header_t);

        switch (header->tipo) {
            case MSG_TELEMETRIA:
                processar_telemetria(sockfd, &cli_addr, len, (payload_telemetria_t*) payload);
                break;
            case MSG_CONCLUSAO:
                processar_conclusao(sockfd, &cli_addr, len, (payload_conclusao_t*) payload);
                break;
            case MSG_ACK:
                printf("[ACK RECEBIDO] Cliente confirmou recebimento de ordem.\n");
                break;
            default:
                printf("Tipo de mensagem desconhecido: %d\n", header->tipo);
        }
    }

    return 0;
}

void carregar_grafo(const char* filename) {
    FILE* f = fopen(filename, "r");
    if (!f) {
        perror("Erro ao abrir arquivo do grafo");
        exit(EXIT_FAILURE);
    }

    fscanf(f, "%d %d", &num_cidades, &num_arestas);

    for(int i=0; i<NUM_CIDADES; i++) {
        for(int j=0; j<NUM_CIDADES; j++) {
            if (i==j) adj[i][j] = 0;
            else adj[i][j] = INFINITO;
        }
        cidades[i].status_atual = STATUS_OK;
        cidades[i].missao_ativa = 0;
    }

    for (int i = 0; i < num_cidades; i++) {
        int id;
        fscanf(f, "%d", &id);

        char temp_line[256];
        fgets(temp_line, sizeof(temp_line), f);

        int len = strlen(temp_line);
        int p = len - 1;
        while(p >= 0 && (temp_line[p] == '\n' || temp_line[p] == ' ' || temp_line[p] == '\r')) p--;

        cidades[id].tipo = temp_line[p] - '0';
        temp_line[p] = '\0';

        p--;
        while(p >= 0 && temp_line[p] == ' ') {
            temp_line[p] = '\0';
            p--;
        }

        char* start = temp_line;
        while(*start == ' ') start++;

        strcpy(cidades[id].nome, start);
        cidades[id].id = id;
    }

    for (int i = 0; i < num_arestas; i++) {
        int u, v, w;
        fscanf(f, "%d %d %d", &u, &v, &w);
        adj[u][v] = w;
        adj[v][u] = w;
    }

    fclose(f);
    printf("Grafo carregado: %d cidades, %d arestas.\n", num_cidades, num_arestas);
}

void processar_telemetria(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, payload_telemetria_t* payload) {
    printf("\n[TELEMETRIA RECEBIDA]\n");
    printf("Total monitorado: %d\n", payload->total);
    
    int alertas_detectados = 0;

    for (int i = 0; i < payload->total; i++) {
        int id = payload->dados[i].id_cidade;
        int status = payload->dados[i].status;

        cidades[id].status_atual = status;

        if (status == STATUS_ALERTA) {
            printf("ALERTA: %s (ID=%d)\n", cidades[id].nome, id);
            alertas_detectados++;
        }
    }

    enviar_ack(sockfd, cli_addr, cli_len, 0);

    if (alertas_detectados > 0) {
        printf("\n[PROCESSANDO ALERTAS]\n");
        for (int i = 0; i < NUM_CIDADES; i++) {
            if (cidades[i].status_atual == STATUS_ALERTA && cidades[i].missao_ativa == 0) {
                despachar_drone(sockfd, cli_addr, cli_len, i);
            }
        }
    }
}

void despachar_drone(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, int id_cidade_alerta) {
    printf("Cidade em alerta: %s (ID=%d)\n", cidades[id_cidade_alerta].nome, id_cidade_alerta);
    
    int id_equipe = -1;
    int dist = executar_dijkstra(id_cidade_alerta, &id_equipe);

    if (id_equipe != -1) {
        printf("-> Dijkstra: capital %s (ID=%d) selecionada, dist=%d km\n", cidades[id_equipe].nome, id_equipe, dist);

        header_t head;
        head.tipo = MSG_EQUIPE_DRONE;
        head.tamanho = sizeof(payload_equipe_drone_t);
        
        payload_equipe_drone_t body;
        body.id_cidade = id_cidade_alerta;
        body.id_equipe = id_equipe;

        char buffer[sizeof(header_t) + sizeof(payload_equipe_drone_t)];
        memcpy(buffer, &head, sizeof(header_t));
        memcpy(buffer + sizeof(header_t), &body, sizeof(payload_equipe_drone_t));

        sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)cli_addr, cli_len);
        printf("-> Ordem enviada: Equipe %s -> %s\n", cidades[id_equipe].nome, cidades[id_cidade_alerta].nome);

        cidades[id_cidade_alerta].missao_ativa = 1;
        equipe_ocupada[id_equipe] = 1;
    } else {
        printf("-> ATENCAO: Nenhuma equipe de drones disponivel no momento!\n");
    }
}

void processar_conclusao(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, payload_conclusao_t* payload) {
    printf("\n[MISSAO CONCLUIDA]\n");
    printf("Cidade atendida: %s (ID=%d)\n", cidades[payload->id_cidade].nome, payload->id_cidade);
    printf("Equipe: %s (ID=%d)\n", cidades[payload->id_equipe].nome, payload->id_equipe);

    cidades[payload->id_cidade].missao_ativa = 0;
    cidades[payload->id_cidade].status_atual = STATUS_OK;
    equipe_ocupada[payload->id_equipe] = 0;

    printf("-> Equipe %s liberada\n", cidades[payload->id_equipe].nome);
    
    enviar_ack(sockfd, cli_addr, cli_len, 2);
}

void enviar_ack(int sockfd, struct sockaddr_in* cli_addr, socklen_t cli_len, int tipo_ack) {
    header_t head;
    head.tipo = MSG_ACK;
    head.tamanho = sizeof(payload_ack_t);
    
    payload_ack_t body;
    body.status = tipo_ack;

    char buffer[sizeof(header_t) + sizeof(payload_ack_t)];
    memcpy(buffer, &head, sizeof(header_t));
    memcpy(buffer + sizeof(header_t), &body, sizeof(payload_ack_t));

    sendto(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)cli_addr, cli_len);
    printf("-> ACK enviado (tipo=%d)\n", tipo_ack);
}

int executar_dijkstra(int origem, int* id_melhor_equipe) {
    int dist[NUM_CIDADES];
    int visitado[NUM_CIDADES];

    for (int i = 0; i < NUM_CIDADES; i++) {
        dist[i] = INFINITO;
        visitado[i] = 0;
    }

    dist[origem] = 0;

    for (int count = 0; count < NUM_CIDADES - 1; count++) {
        int u = -1, min = INFINITO;

        for (int v = 0; v < NUM_CIDADES; v++) {
            if (!visitado[v] && dist[v] <= min) {
                min = dist[v];
                u = v;
            }
        }

        if (u == -1 || dist[u] == INFINITO) break;

        visitado[u] = 1;

        for (int v = 0; v < NUM_CIDADES; v++) {
            if (!visitado[v] && adj[u][v] != INFINITO && dist[u] != INFINITO && dist[u] + adj[u][v] < dist[v]) {
                dist[v] = dist[u] + adj[u][v];
            }
        }
    }

    int menor_dist = INFINITO;
    *id_melhor_equipe = -1;

    for (int i = 0; i < NUM_CIDADES; i++) {
        if (cidades[i].tipo == 1 && equipe_ocupada[i] == 0) {
            if (dist[i] < menor_dist) {
                menor_dist = dist[i];
                *id_melhor_equipe = i;
            }
        }
    }

    return menor_dist;
}