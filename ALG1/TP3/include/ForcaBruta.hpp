#ifndef FORCABRUTA_HPP
#define FORCABRUTA_HPP

#include <vector>
#include <string>
#include <limits>
#include <utility>

class ForcaBruta {
private:
    int numCidades;
    std::vector<std::string> cidades;
    std::vector<std::vector<int>> distancias;
    int menorCusto;
    std::vector<int> melhorRota;

    void permutar(std::vector<int>& caminho, int l, int r);
    int calcularCusto(const std::vector<int>& caminho);

public:
    ForcaBruta(int numCidades, const std::vector<std::string>& cidades, const std::vector<std::vector<int>>& distancias);
    void trocar(int& a, int& b);
    void encontrarMelhorRota();
    int getMenorCusto() const;
    std::vector<std::string> getMelhorRota() const;
};

#endif
