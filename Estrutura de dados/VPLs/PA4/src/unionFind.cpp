#include "unionFind.hpp"
#include <algorithm>
#include <vector>

using namespace std;

UnionFind::UnionFind(int quantidade_subconjuntos) {
    tamanho = quantidade_subconjuntos;
    subconjuntos = new Subconjunto[tamanho];
    for (int i = 0; i < tamanho; i++) {
        Make(i);
    }
}

UnionFind::~UnionFind() {
    delete[] subconjuntos;
}

void UnionFind::Make(int x) {
    subconjuntos[x].representante = x;
    subconjuntos[x].rank = 0;
}

int UnionFind::Find(int x) {
    if (subconjuntos[x].representante != x) {
        subconjuntos[x].representante = Find(subconjuntos[x].representante);
    }
    return subconjuntos[x].representante;
}

void UnionFind::Union(int x, int y) {
    int raizX = Find(x);
    int raizY = Find(y);

    if (raizX != raizY) {
        if (subconjuntos[raizX].rank < subconjuntos[raizY].rank) {
            subconjuntos[raizX].representante = raizY;
        } else if (subconjuntos[raizX].rank > subconjuntos[raizY].rank) {
            subconjuntos[raizY].representante = raizX;
        } else {
            subconjuntos[raizY].representante = raizX;
            subconjuntos[raizX].rank++;
        }
    }
}
