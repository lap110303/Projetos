#ifndef FILA_PRIORIDADE_HPP
#define FILA_PRIORIDADE_HPP

struct NodeFilaPrioridade {
    int vertice;
    double prioridade;
    int contadorPortal;
    NodeFilaPrioridade* next;
};

class FilaPrioridade {
public:
    FilaPrioridade(); //Inicializa os valores da fila de prioridade.
    ~FilaPrioridade(); //Limpa a memória utilizada na construção.

    void adiciona(int vertice, double prioridade, int contadorPortal); //Insere um vértice na fila de prioridade.
    void retira(); //Retira um vértice da fila de prioridade.
    bool vazio(); //Retorna se a fila está vazia ou não.
    NodeFilaPrioridade* topo(); //Retorna o elemento no topo da fila

private:
    NodeFilaPrioridade* head;
};

#endif