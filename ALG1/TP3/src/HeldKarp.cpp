#include "HeldKarp.hpp"
#include <vector>
#include <limits>
#include <algorithm>

HeldKarp::HeldKarp(int numVertices, const std::vector<std::vector<int>>& dist)
    : numVertices(numVertices), dist(dist), menorCusto(std::numeric_limits<int>::max()) { }

void HeldKarp::resolver() {
    int n = numVertices;
    int N = n - 1; 
    int size = 1 << N; // Número de subconjuntos dos vértices 1..n-1
    const int INF = std::numeric_limits<int>::max() / 2;

    // dp[mask][j]: custo mínimo para alcançar a cidade j tendo visitado o conjunto de cidades representado por mask
    std::vector<std::vector<int>> dp(size, std::vector<int>(n, INF));
    // parent[mask][j]: armazena a cidade anterior para reconstruir o caminho
    std::vector<std::vector<int>> parent(size, std::vector<int>(n, -1));

    // Caso base: partindo da cidade 0 para cada cidade j (j de 1 a n-1)
    for (int j = 1; j < n; j++) {
        int mask = 1 << (j - 1);
        dp[mask][j] = dist[0][j];
    }

    // Preenche a tabela dp para cada subconjunto dos vértices
    for (int mask = 0; mask < size; mask++) {
        for (int j = 1; j < n; j++) {
            // Se a cidade j não está no subconjunto 'mask', pula
            if (!(mask & (1 << (j - 1)))) continue;
            // Tenta estender o caminho para uma cidade k que ainda não foi visitada
            for (int k = 1; k < n; k++) {
                if (mask & (1 << (k - 1))) continue; // k já foi visitada
                int nextMask = mask | (1 << (k - 1));
                int newCost = dp[mask][j] + dist[j][k];
                if (newCost < dp[nextMask][k]) {
                    dp[nextMask][k] = newCost;
                    parent[nextMask][k] = j;
                }
            }
        }
    }

    // Finaliza o ciclo voltando para a cidade 0
    int fullMask = size - 1;
    menorCusto = INF;
    int lastCity = -1;
    for (int j = 1; j < n; j++) {
        int cost = dp[fullMask][j] + dist[j][0];
        if (cost < menorCusto) {
            menorCusto = cost;
            lastCity = j;
        }
    }

    // Reconstrução do caminho usando a tabela parent:
    std::vector<int> path;
    int mask = fullMask;
    int current = lastCity;
    while (current != -1) {
        path.push_back(current);
        int temp = parent[mask][current];
        mask = mask & ~(1 << (current - 1));
        current = temp;
    }
    std::reverse(path.begin(), path.end());

    // Monta o melhor caminho: começamos na cidade 0, seguimos pelo caminho reconstruído e voltamos para a origem.
    melhorCaminho.clear();
    melhorCaminho.push_back(0);
    for (int city : path) {
        melhorCaminho.push_back(city);
    }
    melhorCaminho.push_back(0);
}

int HeldKarp::getMenorCusto() const {
    return menorCusto;
}

std::vector<std::string> HeldKarp::getMelhorCaminho(const std::vector<std::string>& cidades) const {
    std::vector<std::string> caminhoStr;
    // Se a primeira e a última cidade forem iguais, omitimos a última.
    int tam = melhorCaminho.size();
    if(tam > 1 && melhorCaminho.front() == melhorCaminho.back()){
        tam--; // ignorar o último elemento
    }
    for (int i = 0; i < tam; i++) {
        caminhoStr.push_back(cidades[melhorCaminho[i]]);
    }
    return caminhoStr;
}
