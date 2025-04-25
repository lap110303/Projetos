#include <iostream>
#include <vector>
#include "unionFind.hpp"

using namespace std;

bool comparaArestas(Aresta a, Aresta b) {
    return a.custo < b.custo;
}

int arvoreGeradoraMinima(int n, int m, vector<Aresta>& arestas) {
    int custoTotal = 0;

    sort(arestas.begin(), arestas.end(), comparaArestas);

    UnionFind dsu(n);

    for (int i = 0; i < m; i++) {
        int u = arestas[i].u;
        int v = arestas[i].v;
        int custo = arestas[i].custo;

        if (dsu.Find(u) != dsu.Find(v)) {
            dsu.Union(u, v);
            custoTotal += custo;
        }
    }

    return custoTotal;
}

int main() {
    int n, m;
    cin >> n >> m;

    vector<Aresta> arestas(m);

    for (int i = 0; i < m; i++) {
        cin >> arestas[i].u >> arestas[i].v >> arestas[i].custo;
    }

    int resultado = arvoreGeradoraMinima(n, m, arestas);

    cout << resultado << endl;

    return 0;
}
