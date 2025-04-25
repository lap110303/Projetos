#ifndef VIZINHOMAISPROX_HPP
#define VIZINHOMAISPROX_HPP

#include <vector>
#include <string>

class VizinhoMaisProx {
private:
    int numCidades;
    std::vector<std::string> cidades;
    std::vector<std::vector<int>> distancias;
    int custoTotal;
    std::vector<int> rota; // Armazena os índices das cidades na ordem da rota

public:
    VizinhoMaisProx(int numCidades, const std::vector<std::string>& cidades, const std::vector<std::vector<int>>& distancias);
    void resolver();
    int getCustoTotal() const;
    std::vector<std::string> getRota() const;
};

#endif
