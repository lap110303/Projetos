#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <vector>
#include <string>
#include <unordered_map>

class Graph {
public:
    Graph(int numCenters);

    void addCenter(const std::string& name, int index);
    void addEdge(const std::string& from, const std::string& to);
    std::string findCapital();
    
    int getCenterIndex(const std::string& name) const;
    int getTotalCenters() const;

    // Tornando públicos os membros que são necessários para a classe Batalhao
    std::vector<std::vector<int>> adjList;
    std::vector<std::string> indexCenter;
    std::unordered_map<std::string, int> centerIndex;

    // Função adicional para acessar o nome do centro
    std::string getCenterName(int index) const {
        // Comparação entre index e o tamanho do vetor usando o mesmo tipo (size_t)
        if (index >= 0 && static_cast<size_t>(index) < indexCenter.size()) {
            return indexCenter[index];
        }
        return "";
    }

};

#endif // GRAFO_HPP
