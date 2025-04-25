#include "graph.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    int n;
    std::cin >> n;
    Grafo grafo;

    for (int i = 0; i < n; ++i) {
        int m;
        std::cin >> m;
        for (int j = 0; j < m; ++j) {
            int vizinho;
            std::cin >> vizinho;
            grafo.InsereAresta(i, vizinho);
        }
    }

    if (std::string(argv[1]) == "-d") {
        std::cout << grafo.QuantidadeVertices() << std::endl;
        std::cout << grafo.QuantidadeArestas() << std::endl;
        std::cout << grafo.GrauMinimo() << std::endl;
        std::cout << grafo.GrauMaximo() << std::endl;
    } else if (std::string(argv[1]) == "-n") {
        for (int i = 0; i < n; ++i) {
            grafo.ImprimeVizinhos(i);
        }
    } else if (std::string(argv[1]) == "-k") {
        int numArestasEsperadas = n * (n - 1) / 2;
        if (grafo.QuantidadeArestas() == numArestasEsperadas) {
            std::cout << 1 << std::endl;
        } else {
            std::cout << 0 << std::endl;
        }
    }

    return 0;
}
