#include "patrulha.hpp"
#include <stack>
#include <queue>

Patrulha::Patrulha(const Graph& g) : grafo(g) {}

bool Patrulha::verificarCircuitoEuleriano(const std::vector<int>& componentes) {
    std::vector<int> grauEntrada(grafo.getTotalCenters(), 0);
    std::vector<int> grauSaida(grafo.getTotalCenters(), 0);

    for (int u : componentes) {
        for (int v : grafo.adjList[u]) {
            grauSaida[u]++;
            grauEntrada[v]++;
        }
    }

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
    rota.push_back(grafo.getCenterName(u));
}

bool Patrulha::ehArestaBidimensional(int u, int v) {
    for (int vizinho : grafo.adjList[v]) {
        if (vizinho == u) {
            return true;
        }
    }
    return false;
}

void Patrulha::explorarRotasBidimensionais(int start, std::vector<std::vector<std::string>>& rotas) {
    std::vector<std::vector<int>> adjListBidimensional(grafo.getTotalCenters());
    for (int u = 0; u < grafo.getTotalCenters(); ++u) {
        for (int v : grafo.adjList[u]) {
            if (ehArestaBidimensional(u, v)) {
                adjListBidimensional[u].push_back(v);
            }
        }
    }

    std::vector<std::vector<int>> adjListTemp = adjListBidimensional;
    std::vector<std::string> rota;

    dfsEuler(start, adjListTemp, rota);

    if (rota.size() > 1) {
        if (rota.front() == rota.back()) {
            rota.pop_back();
        }
        rotas.push_back(std::move(rota));
    }
}

void Patrulha::encontrarRotas(int batalhaoIndex, std::vector<std::vector<std::string>>& rotas) {
    bool possuiBidimensionais = false;
    for (int u = 0; u < grafo.getTotalCenters(); ++u) {
        for (int v : grafo.adjList[u]) {
            if (ehArestaBidimensional(u, v)) {
                possuiBidimensionais = true;
                break;
            }
        }
        if (possuiBidimensionais) break;
    }

    if (possuiBidimensionais) {
        explorarRotasBidimensionais(batalhaoIndex, rotas);
    }

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

    if (verificarCircuitoEuleriano(componentes)) {
        std::vector<std::vector<int>> adjListTemp(grafo.getTotalCenters());
        for (int u : componentes) {
            for (int v : grafo.adjList[u]) {
                adjListTemp[u].push_back(v);
            }
        }

        std::vector<std::string> rota;
        dfsEuler(batalhaoIndex, adjListTemp, rota);

        if (rota.size() > 1 && rota.front() == rota.back()) {
            rota.pop_back();
        }

        if (rota.size() > 1) {
            rotas.push_back(std::move(rota));
        }
    }
}
