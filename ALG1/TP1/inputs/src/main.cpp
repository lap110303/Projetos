#include <iostream>
#include <vector>
#include "capital.hpp"
#include "batalhao.hpp"
#include "patrulha.hpp"

int main() {
    int numCenters, numEdges;
    std::cin >> numCenters >> numEdges;

    Graph g(numCenters);

    // Ler e adicionar estradas
    for (int i = 0; i < numEdges; ++i) {
        std::string from, to;
        std::cin >> from >> to;

        // Adicionar centros urbanos apenas se ainda não estiverem no grafo
        if (g.getCenterIndex(from) == -1) {
            g.addCenter(from, g.getTotalCenters());
        }
        if (g.getCenterIndex(to) == -1) {
            g.addCenter(to, g.getTotalCenters());
        }

        g.addEdge(from, to);
    }

    // Encontrar e imprimir apenas o nome da capital
    std::string capitalName = g.findCapital();
    std::cout << capitalName << std::endl;

    // Criar objeto da classe Batalhao e calcular os batalhões secundários
    Batalhao batalhao(g);
    std::vector<std::string> centrosSecundarios;
    int batalhoesSecundarios = batalhao.calcularBatalhoesSecundarios(g.getCenterIndex(capitalName), centrosSecundarios);

    // Imprimir o número de batalhões secundários
    std::cout << batalhoesSecundarios << std::endl;

    // Agora imprimir as cidades que representam os batalhões secundários
    for (const std::string& centro : centrosSecundarios) {
        std::cout << centro << std::endl;
    }

    // Criar objeto da classe Patrulha e verificar os patrulhamentos
    Patrulha patrulha(g);
    std::vector<std::vector<std::string>> rotasPatrulhamento;

    // Verificar patrulhamento a partir da capital
    patrulha.encontrarRotas(g.getCenterIndex(capitalName), rotasPatrulhamento);

    // Verificar patrulhamento a partir dos batalhões secundários
    for (const std::string& centro : centrosSecundarios) {
        patrulha.encontrarRotas(g.getCenterIndex(centro), rotasPatrulhamento);
    }

    // Imprimir o número de patrulhamentos possíveis
    std::cout << rotasPatrulhamento.size() << std::endl;

    // Imprimir cada rota de patrulhamento válida
    for (const auto& rota : rotasPatrulhamento) {
        for (size_t i = 0; i < rota.size() - 1; ++i) {
            if (i > 0) std::cout << " ";
            std::cout << rota[i];
        }
        std::cout << std::endl;
    }

    return 0;
}
