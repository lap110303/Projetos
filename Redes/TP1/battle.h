#ifndef BATTLE_H
#define BATTLE_H

#include <stdint.h>

#define MSG_SIZE 256

typedef enum {
    MSG_INIT,          // Boas vindas
    MSG_ACTION_REQ,    // Solicita ação ao cliente
    MSG_ACTION_RES,    // Cliente responde com ação escolhida
    MSG_BATTLE_RESULT, // Resultado do turno
    MSG_INVENTORY,     // (opcional) inventário/atualização
    MSG_GAME_OVER,     // Fim de jogo
    MSG_ESCAPE         // Jogador fugiu (não obrigatório)
} MessageType;

typedef struct {
    int type;
    int client_action;
    int server_action;
    int client_hp;
    int server_hp;
    int client_torpedoes;
    int client_shields;
    char message[MSG_SIZE];
} BattleMessage;

#endif
