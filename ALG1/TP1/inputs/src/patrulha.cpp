#include "patrulha.hpp"
#include <stack>
#include <queue>

Patrulha::Patrulha(const Graph& g) : grafo(g) {}

bool Patrulha::verificarCircuitoEuleriano(const std::vector<int>& componentes) {
    std::vector<int> grauEntrada(grafo.getTotalCenters(), 0);
    std::vector<int> grauSaida(grafo.getTotalCenters(), 0);

    // Calcular os graus de entrada e saída
    for (int u : componentes) {
        for (int v : grafo.adjList[u]) {
            grauSaida[u]++;
            grauEntrada[v]++;
        }
    }

    // Verificar se o circuito Euleriano é possível: todos os vértices precisam ter grau de entrada igual ao grau de saída
    for (int u : componentes) {
        if (grauEntrada[u] != grauSaida[u]) {
            return false;
        }
    }
    return true;
}


void Patrulha::dfsEuler(int u, std::vector<std::vector<int>>& adjListTemp, std::vector<std::string>& rota) {
    while (!adjListTemp[u].empty()) {
        int v = adjListTemp[u].back();
        adjListTemp[u].pop_back();
        dfsEuler(v, adjListTemp, rota);
    }
    rota.push_back(grafo.getCenterName(u));  // Adicionar o centro urbano à rota
}


void Patrulha::encontrarRotas(int batalhaoIndex, std::vector<std::vector<std::string>>& rotas) {
    // Passo 1: Identificar o componente fortemente conectado
    std::vector<int> componentes;
    std::queue<int> fila;
    std::vector<bool> visitado(grafo.getTotalCenters(), false);

    fila.push(batalhaoIndex);
    visitado[batalhaoIndex] = true;

    while (!fila.empty()) {
        int atual = fila.front();
        fila.pop();
        componentes.push_back(atual);

        for (int vizinho : grafo.adjList[atual]) {
            if (!visitado[vizinho]) {
                visitado[vizinho] = true;
                fila.push(vizinho);
            }
        }
    }

    // Passo 2: Verificar se é possível formar um circuito Euleriano
    if (verificarCircuitoEuleriano(componentes)) {
        // Copiar temporariamente a lista de adjacências para o componente
        std::vector<std::vector<int>> adjListTemp(grafo.getTotalCenters());
        for (int u : componentes) {
            for (int v : grafo.adjList[u]) {
                adjListTemp[u].push_back(v);
            }
        }

        // Passo 3: Construir a rota usando DFS Euler
        std::vector<std::string> rota;
        dfsEuler(batalhaoIndex, adjListTemp, rota);

        // Adicionar a rota encontrada
        rotas.push_back(std::move(rota));
    }
}
