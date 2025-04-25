#ifndef LISTA_ADJACENCIA_HPP
#define LISTA_ADJACENCIA_HPP

#include "listaEncadeada.hpp"

class ListaAdjacencia {
public:
    ListaAdjacencia(int n);
    ~ListaAdjacencia();
    void adicionarAresta(int v, int w);
    const ListaEncadeada& obterVizinhos(int v) const;
    int obterGrau(int v) const;

private:
    ListaEncadeada* adj;
    int numVertices;
};

#endif
