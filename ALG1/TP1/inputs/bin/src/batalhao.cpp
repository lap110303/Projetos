#include "batalhao.hpp"
#include <stack>
#include <queue>
#include <climits>  // Para usar INT_MAX

Batalhao::Batalhao(const Graph& g) : grafo(g) {
    int n = grafo.getTotalCenters();
    adjListReverso.resize(n);

    // Invertendo as arestas do grafo para o cálculo da segunda DFS
    for (int u = 0; u < n; ++u) {
        for (int v : grafo.adjList[u]) {
            adjListReverso[v].push_back(u);  // Arestas invertidas
        }
    }
}

// Função auxiliar para a primeira DFS: busca em profundidade para ordenação
void Batalhao::dfsOrdenacao(int v, std::vector<bool>& visitado, std::vector<int>& ordem) {
    visitado[v] = true;
    for (int u : grafo.adjList[v]) {
        if (!visitado[u]) {
            dfsOrdenacao(u, visitado, ordem);
        }
    }
    ordem.push_back(v);  // Coloca o vértice no final da ordem após todas as visitas
}

// Função auxiliar para a segunda DFS: busca em profundidade no grafo invertido
void Batalhao::dfsKosaraju(int v, std::vector<bool>& visitado, std::vector<int>& componente, const std::vector<std::vector<int>>& adjList) {
    visitado[v] = true;
    componente.push_back(v);
    for (int u : adjList[v]) {
        if (!visitado[u]) {
            dfsKosaraju(u, visitado, componente, adjList);
        }
    }
}

// Função para calcular a menor distância da capital para cada nó usando BFS
std::vector<int> Batalhao::calcularDistancias(int capitalIndex) {
    int n = grafo.getTotalCenters();
    std::vector<int> distancias(n, INT_MAX);
    std::queue<int> fila;

    distancias[capitalIndex] = 0;
    fila.push(capitalIndex);

    while (!fila.empty()) {
        int u = fila.front();
        fila.pop();

        for (int v : grafo.adjList[u]) {
            if (distancias[v] == INT_MAX) {
                distancias[v] = distancias[u] + 1;
                fila.push(v);
            }
        }
    }

    return distancias;
}

// Função para reverter um vetor manualmente
void reverseVector(std::vector<int>& vec) {
    int left = 0;
    int right = vec.size() - 1;
    while (left < right) {
        std::swap(vec[left], vec[right]);
        ++left;
        --right;
    }
}

int Batalhao::calcularBatalhoesSecundarios(int capitalIndex, std::vector<std::string>& centrosSecundarios) {
    int n = grafo.getTotalCenters();
    std::vector<bool> visitado(n, false);
    std::vector<int> ordem;

    // Primeira DFS para obter a ordem topológica
    for (int i = 0; i < n; ++i) {
        if (!visitado[i]) {
            dfsOrdenacao(i, visitado, ordem);
        }
    }

    // Segunda DFS para encontrar componentes fortemente conectados
    std::fill(visitado.begin(), visitado.end(), false);
    reverseVector(ordem);  // Reverte a ordem para começar pela mais distante
    std::vector<std::vector<int>> componentes;

    for (int v : ordem) {
        if (!visitado[v]) {
            std::vector<int> componente;
            dfsKosaraju(v, visitado, componente, adjListReverso);
            componentes.push_back(componente);
        }
    }

    // Calcular distâncias a partir da capital
    std::vector<int> distancias = calcularDistancias(capitalIndex);

    // Verificar os componentes fortemente conectados e identificar os batalhões secundários
    int batalhoesSecundarios = 0;
    for (const auto& componente : componentes) {
        bool temCapital = false;
        int menorDistancia = INT_MAX;
        int cidadeEscolhida = -1;

        for (int v : componente) {
            if (v == capitalIndex) {
                temCapital = true;
                break;
            }
            // Encontrar a cidade com menor distância até a capital
            if (distancias[v] < menorDistancia) {
                menorDistancia = distancias[v];
                cidadeEscolhida = v;
            }
        }

        if (!temCapital && cidadeEscolhida != -1) {
            centrosSecundarios.push_back(grafo.getCenterName(cidadeEscolhida));
            batalhoesSecundarios++;
        }
    }

    return batalhoesSecundarios;
}
