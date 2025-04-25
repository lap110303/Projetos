#include "listaAdjacencia.hpp"

ListaAdjacencia::ListaAdjacencia(int n) : numVertices(n) {
    adj = new ListaEncadeada[n];
}

ListaAdjacencia::~ListaAdjacencia() {
    delete[] adj;
}

void ListaAdjacencia::adicionarAresta(int v, int w) {
    if (!adj[v].existeNo(w)) {
        adj[v].adicionarNo(w);
    }
}

const ListaEncadeada& ListaAdjacencia::obterVizinhos(int v) const {
    return adj[v];
}

int ListaAdjacencia::obterGrau(int v) const {
    return adj[v].tamanho();
}
