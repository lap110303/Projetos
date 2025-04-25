#ifndef GRAFO_HPP
#define GRAFO_HPP

#include "listaAdjacencia.hpp"

class Grafo {
public:
    Grafo(int n);
    ~Grafo();
    void InsereVertice();
    void InsereAresta(int v, int w);
    int QuantidadeVertices() const;
    int QuantidadeArestas() const;
    int GrauMinimo() const;
    int GrauMaximo() const;
    void ImprimeVizinhos(int v) const;
    bool ehCompleto() const;

private:
    ListaAdjacencia vertices;
    int numVertices;
    int numArestas;
};

#endif
