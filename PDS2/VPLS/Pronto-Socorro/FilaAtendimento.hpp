#ifndef FILA_H
#define FILA_H

#include "Paciente.hpp"

struct FilaAtendimento{
    
    FilaAtendimento();
    void adicionar_paciente(int prioridade, std::string nome, int idade);
    Paciente* chamar_paciente_por_prioridade();
    Paciente* chamar_paciente_por_idade();
    Paciente* buscar_paciente_mais_velho();
    void estimativa_tempo_para_atendimento(int senha);
    void imprimir_fila();
    
    struct Node {
        Paciente* paciente;
        Node* next;
    };
    
    Node* head;
    Node* tail;
    int proxima_senha;
};

#endif