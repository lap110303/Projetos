#include <iostream>
#include "Grafo.hpp"

int main() {

    // Declaração de variáveis
    int V, E;
    std::cin >> V >> E;
    int total = 0;
    int consumo = 0;
    int naoAtendida = 0;
    int perdida = 0;

    Grafo grafo(V + 2); // Adiciona dois vértices extras: gerador universal (V) e consumidor universal (V+1)

    // Leitura dos vértices e suas demandas
    for (int i = 0; i < V; i++) {
        int tipo, demanda;
        std::cin >> tipo >> demanda;
        grafo.demanda[i] = demanda;

        if (demanda == 0) { // Gerador
            grafo.adicionarConexao(V, i, std::numeric_limits<int>::max()); // Conecta ao gerador universal
        } else if (demanda > 0) { // Consumidor
            grafo.adicionarConexao(i, V + 1, std::numeric_limits<int>::max()); // Conecta ao consumidor universal
        }
    }

    // Leitura das arestas
    for (int i = 0; i < E; i++) {
        int u, v, c;
        std::cin >> u >> v >> c;
        grafo.adicionarConexao(u - 1, v - 1, c); // Ajuste de índice para 0-base
    }

    // Resetar a matriz de fluxo antes de calcular o fluxo máximo diretamente
    grafo.resetFluxo();  // Função para resetar o fluxo

    // Calcular fluxo máximo
    int fluxoMax = grafo.fluxoMaximo(V, V + 1);
    //std::cout << "Fluxo máximo do gerador ao consumidor universal: " << fluxoMax << std::endl;

    // Teste da função de fluxo máximo para todos os consumidores
    for (int i = 0; i < V; i++) {
        if (grafo.demanda[i] > 0) { // Somente consumidores com demanda maior que 0
            int fluxoMaximo = grafo.fluxoMaximoParaDestino(i); // Chama a função para o consumidor i
            if(grafo.demanda[i] - fluxoMaximo <= 0){
                total = total + grafo.demanda[i];
            }
            else{
                total = total + fluxoMaximo;
            }
            consumo = consumo + grafo.demanda[i];
        }
    }


    // Impressão dos valores
    naoAtendida = consumo - total;
    perdida = fluxoMax - total;
    std::cout << total << std::endl;
    std::cout << naoAtendida << std::endl;
    std::cout << perdida << std::endl;

    // Agora, pegamos as arestas saturadas
    std::vector<std::pair<int, int>> saturadas = grafo.getSaturadas();
    std::cout << saturadas.size() << std::endl;

    // Exibe as arestas saturadas
    for (const auto& aresta : saturadas) {
        int i = aresta.first;
        int j = aresta.second;
        std::cout << i + 1 << " " << j + 1 << " " << grafo.capacidade[i][j] << std::endl;
    }


    return 0;
}
