#ifndef GRAPH_MATRIX_HPP
#define GRAPH_MATRIX_HPP

#include <limits>
#include "fila_prioridade.hpp"

class GrafoMatriz {
public:
    GrafoMatriz(int n); // Inicializador.
    ~GrafoMatriz(); // Destrutor.

    void novaAresta(int w, int z, double peso); // Adiciona uma nova aresta com seu peso.
    void novoPortal(int w, int z); // Adiciona um novo portal.
    void setCoordenadas(int w, double x, double y); // Adiciona as coordenadas ao portal.

    int getTamanho(); // Ve o tamanho do grafo.
    double getCoordenadaX(int w); // Recebe a coordenada X.
    double getCoordenadaY(int w); // Recebe a coordenada Y.
    double getArestaPeso(int w, int z); // Retorna o peso de uma aresta que vai de w a z.
    int* getPortais(int w, int& count); // Retorna os portais que saem de um vértice.

private:
    struct Node {
        double x, y;
        int* portais;
        int contadorPortal;
    };

    double** matriz;
    Node* nodes;
    int tamanho;
};

double heuristicaMatriz(double x1, double y1, double x2, double y2); // Calculo da heuristica.
int dijkstraM(GrafoMatriz& grafo, int comeco, int fim, double s, int q); // Algoritmo Djikstra.
int aEstrelaM(GrafoMatriz& grafo, int comeco, int fim, double s, int q); // Algoritmo A*.
void leitorMatriz(GrafoMatriz& grafo, int& n, int& m, int& k, double& s, int& q); // Recebe os parametros de teste.

#endif