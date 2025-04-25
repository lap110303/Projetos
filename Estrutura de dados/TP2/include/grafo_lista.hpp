#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <cmath>
#include <limits>
#include "fila_prioridade.hpp"

struct Aresta {
    int destino;
    double peso;
    Aresta* next;
};

struct Portal {
    int destino;
    Portal* next;
};

class GrafoLista {
public:
    GrafoLista(int n); // Inicializador..
    ~GrafoLista(); // Destrutor.

    void novaAresta(int w, int z, double peso); // Adiciona uma nova aresta com seu peso.
    void novoPortal(int w, int z); // Adiciona um novo portal.
    void setCoordenadas(int w, double x, double y); // Adiciona as coordenadas ao portal.

    int getTamanho(); // Ve o tamanho do grafo.
    double getCoordenadaX(int w); // Recebe a coordenada X.
    double getCoordenadaY(int w); // Recebe a coordenada Y.
    Aresta* getAresta(int w); // Retorna as arestas que saem de um vértice.
    Portal* getPortais(int w); // Retorna os portais que saem de um vértice.

private:
    struct Node {
        double x, y;
        Aresta* arestas;
        Portal* portais;
    };

    Node* nodes;
    int tamanho;
};

double heuristicaLista(double x0, double y0, double x1, double y1); // Calculo da heuristica.
int dijkstraL(GrafoLista& grafo, int comeco, int fim, double s, int q); // Algoritmo Djikstra.
int aEstrelaL(GrafoLista& grafo, int comeco, int fim, double s, int q); // Algoritmo A*.
void leitorLista(GrafoLista& grafo, int& n, int& m, int& k, double& s, int& q); // Recebe os parametros de teste.

#endif