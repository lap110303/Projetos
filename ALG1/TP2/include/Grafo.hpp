#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>
#include <iostream>
#include <queue>
#include <limits>
#include <utility>


class Grafo {
public:
    int V;
    std::vector<std::vector<int>> capacidade; // Matriz de capacidades
    std::vector<std::vector<int>> fluxo;      // Matriz de fluxos
    std::vector<std::vector<int>> arestasCriticas; // Matriz de adjacência para arestas críticas
    std::vector<int> demanda; // Valores necessários para os consumidores
    std::vector<bool> visitado; // Bool para verificação de condições

    
    Grafo(int V);
    void adicionarConexao(int u, int v, int cap);
    bool bfs(int origem, int destino, std::vector<int>& pai);
    int fluxoMaximo(int origem, int destino);
    int fluxoMaximoParaDestino(int destino);
    void resetFluxo();
    //void imprimirArestasCriticas() const;
    std::vector<std::pair<int, int>> getSaturadas();
};

#endif
