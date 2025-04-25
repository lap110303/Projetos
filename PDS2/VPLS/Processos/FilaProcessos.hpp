#ifndef FILA_H
#define FILA_H

#include "Processo.hpp"

struct FilaProcessos{
    
    FilaProcessos();
    ~FilaProcessos();
    void adicionar_processo(std::string nome, int prioridade);
    Processo* remover_processo_maior_prioridade();
    Processo* remover_processo_por_id(int id);
    void estimativa_tempo_para_execucao(int id);
    void imprimir_fila();
    
    struct Node {
        Processo* processo;
        Node* next;
    };
    
    Node* head;
    int proximo_id;
};

#endif