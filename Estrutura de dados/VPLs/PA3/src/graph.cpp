#include "graph.hpp"
#include <algorithm>
#include <iostream>
#include <set>

Grafo::Grafo(int n) : vertices(n), numVertices(n), numArestas(0) {}

Grafo::~Grafo() {}

void Grafo::InsereVertice() {
    // Não implementado
}

void Grafo::InsereAresta(int v, int w) {
    if (v != w && !vertices.obterVizinhos(v).existeNo(w)) {
        vertices.adicionarAresta(v, w);
        vertices.adicionarAresta(w, v);
        numArestas++;
    }
}

int Grafo::QuantidadeVertices() const {
    return numVertices;
}

int Grafo::QuantidadeArestas() const {
    return numArestas;
}

int Grafo::GrauMinimo() const {
    int grauMin = vertices.obterGrau(0);
    for (int i = 1; i < numVertices; i++) {
        grauMin = std::min(grauMin, vertices.obterGrau(i));
    }
    return grauMin;
}

int Grafo::GrauMaximo() const {
    int grauMax = vertices.obterGrau(0);
    for (int i = 1; i < numVertices; i++) {
        grauMax = std::max(grauMax, vertices.obterGrau(i));
    }
    return grauMax;
}

void Grafo::ImprimeVizinhos(int v) const {
    vertices.obterVizinhos(v).imprimir();
}

bool Grafo::ehCompleto() const {
    int grauEsperado = numVertices - 1;
    for (int i = 0; i < numVertices; i++) {
        if (vertices.obterGrau(i) != grauEsperado) {
            return false;
        }
    }
    return true;
}