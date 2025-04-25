#ifndef HELDKARP_HPP
#define HELDKARP_HPP

#include <vector>
#include <string>

class HeldKarp {
private:
    int numVertices;
    std::vector<std::vector<int>> dist;
    int menorCusto;
    std::vector<int> melhorCaminho; // Armazena a sequência de índices das cidades

public:
    HeldKarp(int numVertices, const std::vector<std::vector<int>>& dist);
    void resolver();
    int getMenorCusto() const;
    std::vector<std::string> getMelhorCaminho(const std::vector<std::string>& cidades) const;
};

#endif
