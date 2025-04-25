#ifndef TERRENO_HPP
#define TERRENO_HPP

#include <string>

class Terreno {
private:
    char** mapa; // Matriz que representa o mapa
    int linhas;  // Número de linhas do mapa
    int colunas; // Número de colunas do mapa

    void alocarMapa(); // Aloca memória para o mapa
    void liberarMapa(); // Libera a memória alocada para o mapa

public:
    Terreno(); // Construtor padrão
    ~Terreno(); // Destrutor

    bool carregarMapa(const std::string& arquivo); // Carrega o mapa a partir de um arquivo
    char obterPosicao(int linha, int coluna) const; // Retorna o caractere em uma posição específica
    void atualizarPosicao(int linha, int coluna, char novoValor); // Atualiza o caractere em uma posição específica

    int obterLinhas() const; // Retorna o número de linhas do mapa
    int obterColunas() const; // Retorna o número de colunas do mapa
};

#endif
