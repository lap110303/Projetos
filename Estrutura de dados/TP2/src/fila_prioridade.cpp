#include "fila_prioridade.hpp"

FilaPrioridade::FilaPrioridade() : head(nullptr) {} // Inicializador.

FilaPrioridade::~FilaPrioridade() { // Destrutor.
    while (head) {
        NodeFilaPrioridade* temp = head;
        head = head->next;
        delete temp;
    }
}

void FilaPrioridade::adiciona(int vertice, double prioridade, int contadorPortal) {
    NodeFilaPrioridade* node = new NodeFilaPrioridade{vertice, prioridade, contadorPortal, nullptr};
    // Caso a fila esteja vazia ou um novo nó tenha maior prioridade.
    if (!head || prioridade < head->prioridade) {
        node->next = head;
        head = node;
        return;
    }

    NodeFilaPrioridade* atual = head;

    // Encontra a posição de um novo nó.
    while (atual->next && atual->next->prioridade <= prioridade) {
        atual = atual->next;
    }

    // Insere o novo nó.
    node->next = atual->next;
    atual->next = node;
}

void FilaPrioridade::retira() {
    // Remove o nó no topo da fila.
    if (head) {
        NodeFilaPrioridade* temp = head;
        head = head->next;
        delete temp;
    }
}

bool FilaPrioridade::vazio() {
    // Retorna caso a fila esteja vazia
    return head == nullptr;
}

NodeFilaPrioridade* FilaPrioridade::topo() {
    // Retorna o nó no topo
    return head;
}
