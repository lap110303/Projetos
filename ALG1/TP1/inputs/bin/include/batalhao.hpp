#ifndef BATALHAO_HPP
#define BATALHAO_HPP

#include "capital.hpp"

class Batalhao {
public:
    Batalhao(const Graph& g);
    
    int calcularBatalhoesSecundarios(int capitalIndex, std::vector<std::string>& centrosSecundarios);

private:
    void dfsKosaraju(int v, std::vector<bool>& visitado, std::vector<int>& componente, const std::vector<std::vector<int>>& adjList);
    void dfsOrdenacao(int v, std::vector<bool>& visitado, std::vector<int>& ordem);
    std::vector<int> calcularDistancias(int capitalIndex);
    
    const Graph& grafo;
    std::vector<std::vector<int>> adjListReverso;
};

#endif // BATALHAO_HPP
