#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <errno.h>

#include "battle.h"

#define BACKLOG 1
#define START_HP 100 
#define DAMAGE 20    

/**
 * @brief
 *
 * @param sock 
 * @param msg 
 * @return 
 */

static int send_message(int sock, const BattleMessage *msg) {
    unsigned char buf[sizeof(BattleMessage)];
    uint32_t n; 
    size_t offset = 0;

    n = htonl((uint32_t)msg->type);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->client_action);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->server_action);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->client_hp);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->server_hp);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->client_torpedoes);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    n = htonl((uint32_t)msg->client_shields);
    memcpy(buf + offset, &n, sizeof(n)); offset += sizeof(n);

    memcpy(buf + offset, msg->message, sizeof(msg->message));
    offset += sizeof(msg->message);

    size_t to_send = offset;
    size_t sent = 0;
    while (sent < to_send) {
        ssize_t r = send(sock, buf + sent, to_send - sent, 0);
        if (r <= 0) return -1;
        sent += (size_t)r;
    }
    return 0;
}

/**
 * @brief 
 *
 * @param sock 
 * @param msg 
 * @return 
 */

static int recv_message(int sock, BattleMessage *msg) {
    unsigned char buf[sizeof(BattleMessage)];
    size_t to_read = sizeof(buf);
    size_t read_bytes = 0;

    while (read_bytes < to_read) {
        ssize_t r = recv(sock, buf + read_bytes, to_read - read_bytes, 0);
        if (r <= 0) return -1;
        read_bytes += (size_t)r;
    }

    size_t offset = 0;
    uint32_t n;

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->type = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->client_action = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->server_action = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->client_hp = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->server_hp = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->client_torpedoes = (int)ntohl(n);

    memcpy(&n, buf + offset, sizeof(n)); offset += sizeof(n);
    msg->client_shields = (int)ntohl(n);

    memcpy(msg->message, buf + offset, sizeof(msg->message));
    return 0;
}

/**
 * @brief 
 *
 * @param a 
 * @return 
 */

static const char *action_name(int a) {
    if (a == 0) return "Laser Attack";
    if (a == 1) return "Photon Torpedo";
    if (a == 2) return "Shields Up";
    if (a == 3) return "Cloaking";
    if (a == 4) return "Hyper Jump";
    
    return "Unknown";
}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s v4|v6 <port>\n", argv[0]);
        return 1;
    }

    int family = AF_UNSPEC;
    if (strcmp(argv[1], "v4") == 0) {
        family = AF_INET;
    } else if (strcmp(argv[1], "v6") == 0) {
        family = AF_INET6;
    } else {
        fprintf(stderr, "Primeiro argumento deve ser v4 ou v6\n");
        return 1;
    }
    const char *port = argv[2];

    int listen_fd = -1;
    if (family == AF_INET) {
        listen_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd < 0) { perror("socket"); return 1; }
        
        struct sockaddr_in addr4;
        memset(&addr4, 0, sizeof(addr4));
        addr4.sin_family = AF_INET;
        addr4.sin_addr.s_addr = INADDR_ANY;
        addr4.sin_port = htons((uint16_t)atoi(port));
        
        if (bind(listen_fd, (struct sockaddr*)&addr4, sizeof(addr4)) < 0) {
            perror("bind"); return 1;
        }
    } else {
        listen_fd = socket(AF_INET6, SOCK_STREAM, 0);
        if (listen_fd < 0) { perror("socket"); return 1; }
        
        struct sockaddr_in6 addr6;
        memset(&addr6, 0, sizeof(addr6));
        addr6.sin6_family = AF_INET6;
        addr6.sin6_addr = in6addr_any;
        addr6.sin6_port = htons((uint16_t)atoi(port));
        
        if (bind(listen_fd, (struct sockaddr*)&addr6, sizeof(addr6)) < 0) {
            perror("bind"); return 1;
        }
    }

    if (listen(listen_fd, BACKLOG) < 0) { perror("listen"); return 1; }
    printf("Servidor aguardando conexao na porta %s (%s)...\n", port, argv[1]);

    struct sockaddr_storage cli_addr;
    socklen_t cli_len = sizeof(cli_addr);
    int client_fd = accept(listen_fd, (struct sockaddr*)&cli_addr, &cli_len);
    if (client_fd < 0) { perror("accept"); return 1; }
    printf("Cliente conectado.\n");

    close(listen_fd); 

    srand((unsigned)time(NULL));

    int client_hp = START_HP;
    int server_hp = START_HP;
    int torpedoes_used = 0;
    int shields_used = 0;
    int turns = 0;

    int game_running = 1;

    BattleMessage msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = MSG_INIT;
    msg.client_hp = client_hp;
    msg.server_hp = server_hp;
    snprintf(msg.message, sizeof(msg.message), "Bem-vindo ao StarFleet Protocol! Sua nave: SS-42 Voyager (HP: %d)", client_hp);
    if (send_message(client_fd, &msg) < 0) {
        perror("send init");
        close(client_fd);
        return 1;
    }

    while (game_running) {
        memset(&msg, 0, sizeof(msg));
        msg.type = MSG_ACTION_REQ;
        if (send_message(client_fd, &msg) < 0) {
            perror("send action req");
            game_running = 0;
            continue;
        }

        BattleMessage recv;
        if (recv_message(client_fd, &recv) < 0) {
            fprintf(stderr, "Conexao perdida com o cliente.\n");
            game_running = 0;
            continue;
        }

        if (recv.type != MSG_ACTION_RES) {
            continue;
        }

        int client_action = recv.client_action;
        if (client_action < 0 || client_action > 4) {
            memset(&msg, 0, sizeof(msg));
            msg.type = MSG_BATTLE_RESULT;
            msg.client_action = client_action;
            msg.server_action = -1;
            msg.client_hp = client_hp;
            msg.server_hp = server_hp;
            snprintf(msg.message, sizeof(msg.message), "Erro: escolha invalida! Por favor selecione um valor entre 0 e 4.");
            if (send_message(client_fd, &msg) < 0) {
                game_running = 0;
            }
            continue;
        }

        int server_action = rand() % 5;

        if (client_action == 1) torpedoes_used++;
        if (client_action == 2) shields_used++;
        turns++;

        int damage_to_client = 0;
        int damage_to_server = 0;
        char result[MSG_SIZE];
        result[0] = '\0';

        if (client_action == 4 || server_action == 4) {
            if (client_action == 4 && server_action == 4) {
                snprintf(result, sizeof(result), "Ambos acionaram Hyper Jump! Fuga mutua.");
            } else if (client_action == 4) {
                snprintf(result, sizeof(result), "Voce acionou o Hyper Jump! Sua nave escapou para o hiperespaço.");
            } else { 
                snprintf(result, sizeof(result), "Servidor acionou Hyper Jump! Inimigo fugiu para o hiperespaço.");
            }

            memset(&msg, 0, sizeof(msg));
            msg.type = MSG_GAME_OVER;
            msg.client_action = client_action;
            msg.server_action = server_action;
            msg.client_hp = client_hp;
            msg.server_hp = server_hp;
            msg.client_torpedoes = torpedoes_used;
            msg.client_shields = shields_used;
            snprintf(msg.message, sizeof(msg.message),
                     "%s\nInventario final:\n- HP restante: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n- Turnos jogados: %d\nObrigado por jogar!",
                     result, client_hp, torpedoes_used, shields_used, turns);
            
            send_message(client_fd, &msg);
            game_running = 0;
            continue;
        }

        // Bloco 1: Determina dano AO SERVIDOR (causado pelo cliente)
        if (client_action == 0) { // Cliente usou Laser
            if (server_action == 2) { // Servidor usou Escudos
            } else if (server_action == 1) { // Servidor usou Torpedo
            } else if (server_action == 3) { // Servidor usou Cloaking
                damage_to_server += DAMAGE;
            } else if (server_action == 0) { // Servidor usou Laser
                damage_to_server += DAMAGE;
            }
        } else if (client_action == 1) { // Cliente usou Torpedo
            if (server_action == 2) { // Servidor usou Escudos
            } else if (server_action == 3) { // Servidor usou Cloaking
            } else if (server_action == 0) { // Servidor usou Laser
            } else if (server_action == 1) { // Servidor usou Torpedo
                damage_to_server += DAMAGE;
            }
        } 
        // Bloco 2: Determina dano AO CLIENTE (causado pelo servidor) 
        if (server_action == 0) { // Servidor usou Laser
            if (client_action == 2) { // Cliente usou Escudos
            } else if (client_action == 1) { // Cliente usou Torpedo
            } else if (client_action == 3) { // Cliente usou Cloaking
                damage_to_client += DAMAGE;
            } else if (client_action == 0) { // Cliente usou Laser
                damage_to_client += DAMAGE;
            }
        } else if (server_action == 1) { // Servidor usou Torpedo
            if (client_action == 2) { // Cliente usou Escudos
            } else if (client_action == 3) { // Cliente usou Cloaking
            } else if (client_action == 0) { // Cliente usou Laser
                damage_to_client += DAMAGE;
            } else if (client_action == 1) { // Cliente usou Torpedo
                damage_to_client += DAMAGE;
            }
        }
        // Bloco 3: Correção para "Torpedo (1) vence Laser (0)"
        if (client_action == 1 && server_action == 0) {
            damage_to_server += DAMAGE;
        }
        if (server_action == 1 && client_action == 0) {
            damage_to_client += DAMAGE;
        }
        client_hp -= damage_to_client;
        server_hp -= damage_to_server;


        memset(&msg, 0, sizeof(msg));
        msg.type = MSG_BATTLE_RESULT;
        msg.client_action = client_action;
        msg.server_action = server_action;
        msg.client_hp = (client_hp < 0) ? 0 : client_hp;
        msg.server_hp = (server_hp < 0) ? 0 : server_hp;
        msg.client_torpedoes = torpedoes_used;
        msg.client_shields = shields_used;


        if (damage_to_client > 0 && damage_to_server > 0) {
            snprintf(msg.message, sizeof(msg.message), "Resultado: Ambos receberam %d de dano.", DAMAGE);
        } else if (damage_to_client > 0) {
            snprintf(msg.message, sizeof(msg.message), "Resultado: Voce recebeu %d de dano.", damage_to_client); // Usa o dano real
        } else if (damage_to_server > 0) {
            snprintf(msg.message, sizeof(msg.message), "Resultado: Acerto! Nave inimiga perdeu %d HP.", DAMAGE);
        } else {
            if (client_action == 2 && (server_action == 0 || server_action == 1)) {
                snprintf(msg.message, sizeof(msg.message), "Resultado: Ataque inimigo bloqueado !");
            } else if (client_action == 3 && server_action == 1) {
                snprintf(msg.message, sizeof(msg.message), "Resultado: Ataque inimigo falhou !");
            } else {
                snprintf(msg.message, sizeof(msg.message), "Resultado: Nenhum dano aplicado.");
            }
        }

        if (send_message(client_fd, &msg) < 0) {
            game_running = 0;
            continue;
        }

        if (client_hp <= 0 || server_hp <= 0) {
            memset(&msg, 0, sizeof(msg));
            msg.type = MSG_GAME_OVER;
            msg.client_action = client_action;
            msg.server_action = server_action;
            msg.client_hp = (client_hp < 0) ? 0 : client_hp;
            msg.server_hp = (server_hp < 0) ? 0 : server_hp;
            msg.client_torpedoes = torpedoes_used;
            msg.client_shields = shields_used;

            if (server_hp <= 0 && client_hp > 0) {
                snprintf(msg.message, sizeof(msg.message),
                         "Fim de jogo! Voce derrotou a frota inimiga!\nInventario final:\n- HP restante: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n- Turnos jogados: %d\nObrigado por jogar!",
                         msg.client_hp, torpedoes_used, shields_used, turns);
            } else if (client_hp <= 0 && server_hp > 0) {
                snprintf(msg.message, sizeof(msg.message),
                         "Fim de jogo! Sua nave foi destruida!\nInventario final:\n- HP restante: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n- Turnos jogados: %d\nObrigado por jogar!",
                         msg.client_hp, torpedoes_used, shields_used, turns);
            } else {
                snprintf(msg.message, sizeof(msg.message),
                         "Fim de jogo! Batalha terminou.\nInventario final:\n- HP jogador: %d\n- HP inimigo: %d\n- Torpedos usados: %d\n- Escudos usados: %d\n- Turnos jogados: %d\nObrigado por jogar!",
                         msg.client_hp, msg.server_hp, torpedoes_used, shields_used, turns);
            }
            
            send_message(client_fd, &msg);
            game_running = 0;
        }
    }

    close(client_fd);
    printf("Sessao encerrada.\n");
    return 0;
}