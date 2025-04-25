#include "heap.hpp"
#include <algorithm>

Heap::Heap(int maxsize) : tamanho(0), maxsize(maxsize) {
    data = new int[maxsize + 1];
}

Heap::~Heap() {
    delete[] data;
}

void Heap::Inserir(int x) {
    if (tamanho >= maxsize) {
        throw std::overflow_error("Heap está cheio");
    }
    data[++tamanho] = x;
    HeapifyPorCima(tamanho);
}

int Heap::Remover() {
    if (Vazio()) {
        throw std::underflow_error("Heap está vazio");
    }
    int min = data[1];
    data[1] = data[tamanho--];
    HeapifyPorBaixo(1);
    return min;
}

bool Heap::Vazio() {
    return tamanho == 0;
}

int Heap::GetAncestral(int posicao) {
    return posicao / 2;
}

int Heap::GetSucessorEsq(int posicao) {
    return 2 * posicao;
}

int Heap::GetSucessorDir(int posicao) {
    return 2 * posicao + 1;
}

void Heap::HeapifyPorCima(int posicao) {
    while (posicao > 1 && data[posicao] < data[GetAncestral(posicao)]) {
        std::swap(data[posicao], data[GetAncestral(posicao)]);
        posicao = GetAncestral(posicao);
    }
}

void Heap::HeapifyPorBaixo(int posicao) {
    int menor = posicao;
    int esquerda = GetSucessorEsq(posicao);
    int direita = GetSucessorDir(posicao);

    if (esquerda <= tamanho && data[esquerda] < data[menor]) {
        menor = esquerda;
    }

    if (direita <= tamanho && data[direita] < data[menor]) {
        menor = direita;
    }

    if (menor != posicao) {
        std::swap(data[posicao], data[menor]);
        HeapifyPorBaixo(menor);
    }
}
