#include <iostream>
#include <vector>
#include <set>
#include "capital.hpp"
#include "batalhao.hpp"
#include "patrulha.hpp"

bool isSubRoute(const std::vector<std::string>& subRoute, const std::vector<std::string>& fullRoute) {
    if (subRoute.size() > fullRoute.size()) return false;
    
    for (size_t i = 0; i <= fullRoute.size() - subRoute.size(); ++i) {
        bool match = true;
        for (size_t j = 0; j < subRoute.size(); ++j) {
            if (fullRoute[i + j] != subRoute[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

int main() {
    int numCenters, numEdges;
    std::cin >> numCenters >> numEdges;

    Graph g(numCenters);

    for (int i = 0; i < numEdges; ++i) {
        std::string from, to;
        std::cin >> from >> to;

        if (g.getCenterIndex(from) == -1) {
            g.addCenter(from, g.getTotalCenters());
        }
        if (g.getCenterIndex(to) == -1) {
            g.addCenter(to, g.getTotalCenters());
        }

        g.addEdge(from, to);
    }

    std::string capitalName = g.findCapital();
    std::cout << capitalName << std::endl;

    Batalhao batalhao(g);
    std::vector<std::string> centrosSecundarios;
    int batalhoesSecundarios = batalhao.calcularBatalhoesSecundarios(g.getCenterIndex(capitalName), centrosSecundarios);

    std::cout << batalhoesSecundarios << std::endl;
    for (const std::string& centro : centrosSecundarios) {
        std::cout << centro << std::endl;
    }

    Patrulha patrulha(g);
    std::vector<std::vector<std::string>> rotasPatrulhamento;
    std::set<std::vector<std::string>> rotasUnicas;

    patrulha.encontrarRotas(g.getCenterIndex(capitalName), rotasPatrulhamento);

    for (const std::string& centro : centrosSecundarios) {
        patrulha.encontrarRotas(g.getCenterIndex(centro), rotasPatrulhamento);
    }

    for (const auto& rota : rotasPatrulhamento) {
        rotasUnicas.insert(rota);
    }

    std::vector<std::vector<std::string>> rotasFiltradas;
    for (const auto& rota : rotasUnicas) {
        bool isPartOfAnother = false;
        for (const auto& outraRota : rotasUnicas) {
            if (rota != outraRota && isSubRoute(rota, outraRota)) {
                isPartOfAnother = true;
                break;
            }
        }
        if (!isPartOfAnother) {
            rotasFiltradas.push_back(rota);
        }
    }

    std::cout << rotasFiltradas.size() << std::endl;
    for (const auto& rota : rotasFiltradas) {
        for (size_t i = 0; i < rota.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << rota[i];
        }
        std::cout << std::endl;
    }

    return 0;
}