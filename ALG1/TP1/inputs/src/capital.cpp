#include "capital.hpp"
#include <queue>
#include <limits>

Graph::Graph(int numCenters) {
    adjList.resize(numCenters);
    indexCenter.resize(numCenters);
}

void Graph::addCenter(const std::string& name, int index) {
    if (centerIndex.find(name) == centerIndex.end()) {
        indexCenter[index] = name;
        centerIndex[name] = index;
    }
}

void Graph::addEdge(const std::string& from, const std::string& to) {
    int fromIndex = centerIndex[from];
    int toIndex = centerIndex[to];
    adjList[fromIndex].push_back(toIndex);
}

std::string Graph::findCapital() {
    int n = adjList.size();
    int minDistanceSum = std::numeric_limits<int>::max();
    int capitalIndex = -1;

    for (int i = 0; i < n; ++i) {
        std::vector<int> distance(n, std::numeric_limits<int>::max());
        distance[i] = 0;

        std::queue<int> q;
        q.push(i);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v : adjList[u]) {
                if (distance[v] == std::numeric_limits<int>::max()) {
                    distance[v] = distance[u] + 1;
                    q.push(v);
                }
            }
        }

        int distanceSum = 0;
        bool allReachable = true;

        for (int d : distance) {
            if (d == std::numeric_limits<int>::max()) {
                allReachable = false;
                break;
            }
            distanceSum += d;
        }

        if (allReachable && distanceSum < minDistanceSum) {
            minDistanceSum = distanceSum;
            capitalIndex = i;
        }
    }

    return capitalIndex != -1 ? indexCenter[capitalIndex] : "";
}

int Graph::getCenterIndex(const std::string& name) const {
    if (centerIndex.find(name) != centerIndex.end()) {
        return centerIndex.at(name);
    }
    return -1;  // Retorna -1 se o centro não foi encontrado
}

int Graph::getTotalCenters() const {
    return centerIndex.size();
}
