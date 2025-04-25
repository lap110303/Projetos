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

    // Verificar se o circuito Euleriano é possível
    for (int u : componentes) {
        if (grauEntrada[u] != grauSaida[u]) {
            return false;
        }
    }
    return true;
}

bool Patrulha::pertenceAoVetor(int valor, const std::vector<int>& vetor) {
    for (int elemento : vetor) {
        if (elemento == valor) {
            return true;
        }
    }
    return false;
}

void Patrulha::dfsEuler(int u, std::vector<std::vector<int>>& adjListTemp, std::vector<std::string>& rota) {
    while (!adjListTemp[u].empty()) {
        int v = adjListTemp[u].back();
        adjListTemp[u].pop_back();
        dfsEuler(v, adjListTemp, rota);
    }
    rota.push_back(grafo.getCenterName(u));  // Adiciona a cidade à rota
}

void Patrulha::encontrarRotas(int batalhaoIndex, std::vector<std::vector<std::string>>& rotas) {
    // Passo 1: Identificar o componente fortemente conectado da capital
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

    // Passo 2: Verificar se a capital está isolada sem rotas válidas
    if (componentes.size() == 1 && grafo.adjList[batalhaoIndex].empty()) {
        return; // Não há rotas a serem adicionadas
    }

    // Passo 3: Verificar se o circuito Euleriano é possível
    if (verificarCircuitoEuleriano(componentes)) {
        // Copiar temporariamente a lista de adjacências para o componente
        std::vector<std::vector<int>> adjListTemp(grafo.getTotalCenters());
        for (int u : componentes) {
            for (int v : grafo.adjList[u]) {
                adjListTemp[u].push_back(v);
            }
        }

        // Passo 4: Construir a rota usando DFS Euler
        std::vector<std::string> rota;
        dfsEuler(batalhaoIndex, adjListTemp, rota);

        // Ajustar a rota para remover ciclos triviais (como capital -> capital)
        if (rota.size() > 1 && rota.front() == rota.back()) {
            rota.pop_back(); // Remove a repetição da capital no final
        }

        // Adicionar a rota encontrada
        if (rota.size() > 1) { // Garantir que a rota tenha mais de um elemento
            rotas.push_back(std::move(rota));
        }
    }
}
