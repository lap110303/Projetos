#include "VizinhoMaisProx.hpp"
#include <vector>
#include <limits>

VizinhoMaisProx::VizinhoMaisProx(int numCidades, const std::vector<std::string>& cidades, const std::vector<std::vector<int>>& distancias)
    : numCidades(numCidades), cidades(cidades), distancias(distancias), custoTotal(0) { }

void VizinhoMaisProx::resolver() {
    // Heurística do vizinho mais próximo: partindo da cidade 0
    int atual = 0;
    std::vector<bool> visitado(numCidades, false);
    visitado[atual] = true;
    rota.push_back(atual);
    custoTotal = 0;
    
    // Visita todas as cidades
    for (int cont = 1; cont < numCidades; cont++) {
        int proximo = -1;
        int menorDist = std::numeric_limits<int>::max();
        for (int j = 0; j < numCidades; j++) {
            if (!visitado[j] && distancias[atual][j] < menorDist) {
                menorDist = distancias[atual][j];
                proximo = j;
            }
        }
        if (proximo == -1) break; // Em princípio, não deve ocorrer
        rota.push_back(proximo);
        visitado[proximo] = true;
        custoTotal += menorDist;
        atual = proximo;
    }
    // Volta para a cidade inicial para fechar o ciclo
    custoTotal += distancias[atual][0];
    rota.push_back(0);
}

int VizinhoMaisProx::getCustoTotal() const {
    return custoTotal;
}

std::vector<std::string> VizinhoMaisProx::getRota() const {
    std::vector<std::string> rotaStr;
    // Se a cidade inicial estiver duplicada (início e fim), omitimos a última
    int tam = rota.size();
    if(tam > 1 && rota.front() == rota.back()){
        tam--; // Ignora o último elemento
    }
    for (int i = 0; i < tam; i++) {
        rotaStr.push_back(cidades[rota[i]]);
    }
    return rotaStr;
}
