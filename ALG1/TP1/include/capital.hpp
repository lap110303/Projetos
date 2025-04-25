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

    std::vector<std::vector<int>> adjList;
    std::vector<std::string> indexCenter;
    std::unordered_map<std::string, int> centerIndex;

    std::string getCenterName(int index) const {
        if (index >= 0 && static_cast<size_t>(index) < indexCenter.size()) {
            return indexCenter[index];
        }
        return "";
    }

};

#endif