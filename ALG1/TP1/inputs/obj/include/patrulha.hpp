#ifndef PATRULHA_HPP
#define PATRULHA_HPP

#include "capital.hpp"
#include <vector>
#include <string>

class Patrulha {
public:
    Patrulha(const Graph& g);

    bool verificarCircuitoEuleriano(const std::vector<int>& componentes);
    void encontrarRotas(int batalhaoIndex, std::vector<std::vector<std::string>>& rotas);

private:
    void dfsEuler(int v, std::vector<std::vector<int>>& adjListTemp, std::vector<std::string>& rota);
    bool pertenceAoVetor(int valor, const std::vector<int>& vetor);

    const Graph& grafo;
};

#endif // PATRULHA_HPP
