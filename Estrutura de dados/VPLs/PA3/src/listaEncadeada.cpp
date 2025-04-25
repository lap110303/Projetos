#include "listaEncadeada.hpp"
#include <iostream>

No::No(int valor) : valor(valor), proximo(nullptr) {}

ListaEncadeada::ListaEncadeada() : cabeca(nullptr), tamanhoLista(0) {}

ListaEncadeada::~ListaEncadeada() {
    No* atual = cabeca;
    while (atual != nullptr) {
        No* proximo = atual->proximo;
        delete atual;
        atual = proximo;
    }
}

void ListaEncadeada::adicionarNo(int valor) {
    No* novoNo = new No(valor);
    novoNo->proximo = cabeca;
    cabeca = novoNo;
    tamanhoLista++;
}

bool ListaEncadeada::existeNo(int valor) const {
    No* atual = cabeca;
    while (atual != nullptr) {
        if (atual->valor == valor) {
            return true;
        }
        atual = atual->proximo;
    }
    return false;
}

int ListaEncadeada::tamanho() const {
    return tamanhoLista;
}

void ListaEncadeada::imprimir() const {
    No* atual = cabeca;
    while (atual != nullptr) {
        std::cout << atual->valor << " ";
        atual = atual->proximo;
    }
    std::cout << std::endl;
}

No* ListaEncadeada::primeiro() const {
    return cabeca;
}
