#include "FilaProcessos.hpp"

#include<iostream>
#include <iomanip>

FilaProcessos::FilaProcessos() {
    head = nullptr;
    proximo_id = 1;
}

FilaProcessos::~FilaProcessos() {
    Node* atual = head;
    while (atual != nullptr) {
        Node* proximo = atual->next;
        delete atual->processo;
        delete atual;
        atual = proximo;
    }
}

void FilaProcessos::adicionar_processo(std::string nome, int prioridade) {
    Processo* novo_processo = new Processo(proximo_id++, nome, prioridade);
    Node* novo_nodo = new Node{novo_processo, nullptr};

    if (head == nullptr || head->processo->getPrioridade() < prioridade) {
        novo_nodo->next = head;
        head = novo_nodo;
    } else {
        Node* atual = head;
        while (atual->next != nullptr && atual->next->processo->getPrioridade() >= prioridade) {
            atual = atual->next;
        }
        novo_nodo->next = atual->next;
        atual->next = novo_nodo;
    }
}

Processo* FilaProcessos::remover_processo_maior_prioridade() {
    if (head == nullptr) return nullptr;
    Node* removido = head;
    head = head->next;
    Processo* processo_removido = removido->processo;
    delete removido;
    return processo_removido;
}

Processo* FilaProcessos::remover_processo_por_id(int id) {
    if (head == nullptr) return nullptr;

    Node* atual = head;
    Node* anterior = nullptr;

    while (atual != nullptr && atual->processo->getID() != id) {
        anterior = atual;
        atual = atual->next;
    }

    if (atual == nullptr) return nullptr;

    if (anterior == nullptr) {
        head = atual->next;
    } else {
        anterior->next = atual->next;
    }

    Processo* processo_removido = atual->processo;
    delete atual;
    return processo_removido;
}

void FilaProcessos::estimativa_tempo_para_execucao(int id) {
    Node* atual = head;
    double tempo_estimado = 0.0;
    bool encontrado = false;

    while (atual != nullptr) {
        if (atual->processo->getID() == id) {
            encontrado = true;
            break;
        }
        tempo_estimado += atual->processo->tempo_reservado_processo();
        atual = atual->next;
    }

    if (encontrado) {
        std::cout << std::fixed << std::setprecision(1);
        std::cout << "Tempo estimado para execução do processo " << atual->processo->getNome() 
                  << " (id=" << id << ") eh " << tempo_estimado << " segundos." << std::endl;
    }
}

void FilaProcessos::imprimir_fila() {
    Node* atual = head;
    while (atual != nullptr) {
        atual->processo->imprimir_dados();
        atual = atual->next;
    }
}