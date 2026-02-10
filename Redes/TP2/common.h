#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

// Configurações de Rede
#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

// Constantes do Problema
#define NUM_CIDADES 45
#define PROB_ALERTA 3 // 3%
#define INTERVALO_TELEMETRIA 30 // segundos

// Tipos de Mensagem
#define MSG_TELEMETRIA 1
#define MSG_ACK 2
#define MSG_EQUIPE_DRONE 3
#define MSG_CONCLUSAO 4

// Status
#define STATUS_OK 0
#define STATUS_ALERTA 1

// Cabeçalho Padrão (Usei packed para garantir alinhamento na rede)
typedef struct __attribute__((packed)) {
    uint16_t tipo;
    uint16_t tamanho;
} header_t;

// Payloads
typedef struct __attribute__((packed)) {
    int id_cidade;
    int status; // 0 = OK, 1 = ALERTA
} telemetria_t;

typedef struct __attribute__((packed)) {
    int total;
    telemetria_t dados[50]; // Buffer fixo conforme especificado, cobre as 45 cidades
} payload_telemetria_t;

typedef struct __attribute__((packed)) {
    int status; // 0=ACK TELEMETRIA, 1=ACK DRONE, 2=ACK CONCLUSAO
} payload_ack_t;

typedef struct __attribute__((packed)) {
    int id_cidade; // Cidade com alerta
    int id_equipe; // ID da Capital (Equipe de Drone)
} payload_equipe_drone_t;

typedef struct __attribute__((packed)) {
    int id_cidade;
    int id_equipe;
} payload_conclusao_t;

#endif