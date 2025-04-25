#include "Terreno.hpp"
#include <fstream>
#include <iostream>

Terreno::Terreno() : mapa(nullptr), linhas(0), colunas(0) {} // Construtor

Terreno::~Terreno() { // Destrutor
    liberarMapa();
}

void Terreno::alocarMapa() { // Aloca o espaço do mapa
    mapa = new char*[linhas];
    for (int i = 0; i < linhas; ++i) {
        mapa[i] = new char[colunas];
    }
}

void Terreno::liberarMapa() { // Libera a memória alocada
    if (mapa) {
        for (int i = 0; i < linhas; ++i) {
            delete[] mapa[i];
        }
        delete[] mapa;
        mapa = nullptr;
    }
}

bool Terreno::carregarMapa(const std::string& arquivo) {
    std::ifstream mapaFile(arquivo);
    if (!mapaFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de mapa: " << arquivo << std::endl;
        return false;
    }

    mapaFile >> linhas >> colunas;

    liberarMapa();
    alocarMapa();

    for (int i = 0; i < linhas; ++i) {
        for (int j = 0; j < colunas; ++j) {
            mapaFile >> mapa[i][j];
        }
    }

    mapaFile.close();
    return true; // Sucesso
}

char Terreno::obterPosicao(int linha, int coluna) const { // Verifica a posição na Matriz
    if (linha >= 0 && linha < linhas && coluna >= 0 && coluna < colunas) {
        return mapa[linha][coluna];
    }
    return '\0'; // Retorna caractere nulo se fora dos limites
}

void Terreno::atualizarPosicao(int linha, int coluna, char novoValor) { // Atualiza a posição em caso de operação
    if (linha >= 0 && linha < linhas && coluna >= 0 && coluna < colunas) {
        mapa[linha][coluna] = novoValor;
    }
}

int Terreno::obterLinhas() const { // Número de linhas
    return linhas;
}

int Terreno::obterColunas() const { // Número de colunas
    return colunas;
}