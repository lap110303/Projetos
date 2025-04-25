#include "ForcaBruta.hpp"
#include <limits>

// Construtor: Inicializa o número de cidades, os nomes das cidades, a matriz de distâncias e o menor custo como infinito.
ForcaBruta::ForcaBruta(int numCidades, const std::vector<std::string>& cidades, const std::vector<std::vector<int>>& distancias)
    : numCidades(numCidades), cidades(cidades), distancias(distancias),
      menorCusto(std::numeric_limits<int>::max()) {}

// Função auxiliar para trocar dois elementos (usada para gerar permutações).
void ForcaBruta::trocar(int& a, int& b) {
    int temp = a;
    a = b;
    b = temp;
}

// Gera todas as permutações possíveis do vetor `caminho`.
// A cada permutação gerada, calcula o custo total e verifica se é a menor rota encontrada.
void ForcaBruta::permutar(std::vector<int>& caminho, int l, int r) {
    if (l == r) { 
        // Caso base: uma permutação completa foi gerada
        int custo = calcularCusto(caminho);
        if (custo < menorCusto) {
            menorCusto = custo; // Atualiza o menor custo encontrado
            melhorRota = caminho; // Armazena a rota correspondente
        }
    } else {
        for (int i = l; i <= r; i++) {
            trocar(caminho[l], caminho[i]); // Troca elementos
            permutar(caminho, l + 1, r);   // Chamada recursiva para o próximo índice
            trocar(caminho[l], caminho[i]); // Backtracking para restaurar o vetor original
        }
    }
}

// Calcula o custo total de uma rota específica (caminho).
// Inclui o custo para retornar à cidade inicial.
int ForcaBruta::calcularCusto(const std::vector<int>& caminho) {
    int custo = 0;
    for (size_t i = 0; i < caminho.size() - 1; i++) {
        custo += distancias[caminho[i]][caminho[i + 1]];
    }
    custo += distancias[caminho.back()][caminho[0]]; // Retorno à cidade inicial
    return custo;
}

// Encontra a melhor rota através de permutações.
// Inicializa o vetor `caminho` com os índices das cidades e chama a função `permutar`.
void ForcaBruta::encontrarMelhorRota() {
    std::vector<int> caminho(numCidades);
    for (int i = 0; i < numCidades; i++) {
        caminho[i] = i;
    }
    permutar(caminho, 0, numCidades - 1); // Gera todas as permutações possíveis
}

// Retorna o menor custo encontrado após executar `encontrarMelhorRota`.
int ForcaBruta::getMenorCusto() const {
    return menorCusto;
}

// Retorna a melhor rota encontrada após executar `encontrarMelhorRota`.
// A rota é convertida de índices para os nomes das cidades.
std::vector<std::string> ForcaBruta::getMelhorRota() const {
    std::vector<std::string> rota;
    for (int cidade : melhorRota) {
        rota.push_back(cidades[cidade]);
    }
    return rota;
}
