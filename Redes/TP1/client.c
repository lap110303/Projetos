#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>

#include "battle.h"

/**
 * @brief
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
        fprintf(stderr, "Uso: %s <ip/hostname> <port>\n", argv[0]);
        return 1;
    }
    const char *host = argv[1];
    const char *port = argv[2];

    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;     

    int s = getaddrinfo(host, port, &hints, &res);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        return 1;
    }

    int sock = -1;
    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock == -1) {
            continue; 
        }
        if (connect(sock, rp->ai_addr, rp->ai_addrlen) == 0) {
            break;
        }
        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);
    if (sock == -1) {
        fprintf(stderr, "Nao foi possivel conectar ao servidor\n");
        return 1;
    }

    printf("Conectado ao servidor.\n");

    BattleMessage msg;
    if (recv_message(sock, &msg) < 0) {
        fprintf(stderr, "Conexao encerrada.\n");
        close(sock);
        return 1;
    }
    
    if (msg.type == MSG_INIT) {
        printf("%s\n", msg.message);
        printf("Sua nave : SS-42 Voyager (HP: %d)\n\n", msg.client_hp);
    }

    int game_running = 1;

    while (game_running) {
        if (recv_message(sock, &msg) < 0) {
            fprintf(stderr, "Conexao encerrada pelo servidor.\n");
            game_running = 0;
            continue;
        }

        if (msg.type == MSG_ACTION_REQ) {
            int choice = -1;
            while (1) {
                printf("Escolha sua acao :\n");
                printf("0 - Laser Attack\n1 - Photon Torpedo\n2 - Shields Up\n3 - Cloaking\n4 - Hyper Jump\n\n> ");
                
                if (scanf("%d", &choice) != 1) {
                    int c;
                    while ((c = getchar()) != EOF && c != '\n');
                    printf("Entrada invalida. Digite um numero entre 0 e 4.\n");
                    continue;
                }
                
                if (choice < 0 || choice > 4) {
                    printf("Erro: escolha invalida! Por favor selecione um valor entre 0 e 4.\n\n");
                    continue; 
                }
                
                break;
            }

            BattleMessage out;
            memset(&out, 0, sizeof(out));
            out.type = MSG_ACTION_RES;
            out.client_action = choice;

            if (choice == 0) printf("Voce disparou um Laser !\n");
            else if (choice == 1) printf("Voce disparou um Photon Torpedo !\n");
            else if (choice == 2) printf("Voce ativou os Escudos !\n");
            else if (choice == 3) printf("Voce ativou Cloaking !\n");
            else if (choice == 4) printf("Voce acionou o Hyper Jump !\n");

            if (send_message(sock, &out) < 0) {
                fprintf(stderr, "Erro ao enviar acao.\n");
                game_running = 0;
                continue;
            }

            if (recv_message(sock, &msg) < 0) {
                fprintf(stderr, "Conexao encerrada pelo servidor.\n");
                game_running = 0;
                continue;
            }

            if (msg.type == MSG_BATTLE_RESULT) {
                printf("Servidor usou %s .\n", action_name(msg.server_action));
                printf("%s\n", msg.message); 
                printf("Placar : Voce %d x %d Inimigo\n\n", msg.client_hp, msg.server_hp);
                continue;
            
            } else if (msg.type == MSG_GAME_OVER) {
                printf("%s\n", msg.message); 
                game_running = 0;
                continue;
            
            } else {
                if (msg.type == MSG_GAME_OVER) {
                    printf("%s\n", msg.message);
                    game_running = 0;
                }
            }

        } else if (msg.type == MSG_GAME_OVER) {
            printf("%s\n", msg.message);
            game_running = 0;
        } else {
        }
    }

    close(sock);
    return 0;
}