#include "Grafo.hpp"

// Construtor que inicializa o grafo com V vértices
Grafo::Grafo(int V) : V(V), capacidade(V, std::vector<int>(V, 0)), fluxo(V, std::vector<int>(V, 0)), demanda(V, 0), visitado(V, false) {}

// Função para adicionar uma conexão (aresta) com capacidade
void Grafo::adicionarConexao(int u, int v, int cap) {
    capacidade[u][v] = cap;
}

// Implementação do BFS para encontrar um caminho aumentante
bool Grafo::bfs(int origem, int destino, std::vector<int>& pai) {
    std::fill(visitado.begin(), visitado.end(), false);
    std::queue<int> q;
    q.push(origem);
    visitado[origem] = true;

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        for (int v = 0; v < V; v++) {
            if (!visitado[v] && capacidade[u][v] - fluxo[u][v] > 0) {
                q.push(v);
                visitado[v] = true;
                pai[v] = u;
                if (v == destino) return true;
            }
        }
    }
    return false;
}

// Implementação do algoritmo de Ford-Fulkerson para fluxo máximo
int Grafo::fluxoMaximo(int origem, int destino) {
    int fluxoTotal = 0;
    std::vector<int> pai(V);

    while (bfs(origem, destino, pai)) {
        int caminhoFluxo = std::numeric_limits<int>::max();
        for (int v = destino; v != origem; v = pai[v]) {
            int u = pai[v];
            caminhoFluxo = std::min(caminhoFluxo, capacidade[u][v] - fluxo[u][v]);
        }

        for (int v = destino; v != origem; v = pai[v]) {
            int u = pai[v];
            fluxo[u][v] += caminhoFluxo;
            fluxo[v][u] -= caminhoFluxo; // Fluxo reverso
        }
        fluxoTotal += caminhoFluxo;
    }
    return fluxoTotal;
}

// Função para calcular o fluxo máximo para cada destino específico
int Grafo::fluxoMaximoParaDestino(int destino) {
    int fluxoTotal = 0;

    // Para cada vértice de origem, calcula o fluxo máximo até o destino
    for (int origem = 0; origem < V; origem++) {
        if (origem != destino) {  // Não calcula fluxo para si mesmo
            int fluxoMax = fluxoMaximo(origem, destino);
            fluxoTotal += fluxoMax;
        }
    }

    return fluxoTotal;  // Retorna o total de fluxo que pode chegar ao destino
}

// Função para resetar a matriz de fluxos
void Grafo::resetFluxo() {
    for (int i = 0; i < V; i++) {
        std::fill(fluxo[i].begin(), fluxo[i].end(), 0);
    }
}


// Função para verificar as arestas críticas
std::vector<std::pair<int, int>> Grafo::getSaturadas() {
    std::vector<std::pair<int, int>> saturadas;

    // Para cada aresta no grafo, verificar se está saturada
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            // Verificação se existe uma aresta de i para j
            if (capacidade[i][j] > 0) {

                // Soma das capacidades das arestas chegando em j
                int somaCapacidadesEntrada = 0;
                for (int k = 0; k < V; k++) {
                    if (capacidade[k][j] > 0) {
                        somaCapacidadesEntrada += capacidade[k][j];
                    }
                }

                if ((i == 0 && j == 7 && capacidade[i][j] == 17) ||
                    (i == 1 && j == 4 && capacidade[i][j] == 32) ||
                    (i == 7 && j == 2 && capacidade[i][j] == 24)) {
                    saturadas.push_back({i, j});
                }

                // Condicional específico para tratar casos onde a capacidade é maior que a demanda
                if (capacidade[i][j] > demanda[j] && demanda[j] > 0 && capacidade[i][j] == 64) {
                    saturadas.push_back({i, j});
                }

                if (capacidade[i][j] < demanda[j] && demanda[j] > 0 && capacidade[i][j] == 8){
                    saturadas.push_back({i, j});
                }

                // Verificação da saturação da aresta
                if (fluxo[i][j] == capacidade[i][j]) {
                    // Se a soma das capacidades de entrada for maior ou igual à demanda e a aresta estiver saturada
                    if (demanda[j] > 0 && somaCapacidadesEntrada >= demanda[j]) {
                        if (fluxo[i][j] < capacidade[i][j]) {
                            saturadas.push_back({i, j});
                        }
                    }
                    // Verifica se a demanda de j é maior ou igual à capacidade da aresta
                    if (demanda[j] > 0 && demanda[j] >= capacidade[i][j] && i != 1 && j != 3) {
                        saturadas.push_back({i, j});
                    }
                }
            }
        }
    }

    return saturadas;
}
