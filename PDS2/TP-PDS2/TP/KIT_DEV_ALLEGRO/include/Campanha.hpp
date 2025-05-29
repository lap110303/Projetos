#ifndef CAMPANHA_HPP
#define CAMPANHA_HPP

#include <string>
#include <vector>
#include "Treinador.hpp"

/**
 * @class Campanha
 * 
 * @brief Classe que representa uma campanha de um treinador em um jogo.
 * 
 * A classe Campanha armazena informações sobre o treinador, a quantidade de batalhas,
 * a quantidade de derrotas, os adversários enfrentados e o tempo de jogo.
 * 
 * A classe possui métodos para atualizar o tempo de jogo, registrar vitórias e derrotas,
 * e obter informações sobre a campanha.
 */
class Campanha {
private:
    Treinador *_treinador; /**< Ponteiro para o treinador da campanha */
    int _quantidade_batalhas; /**< Quantidade de batalhas realizadas na campanha */
    int _quantidade_derrotas; /**< Quantidade de derrotas sofridas na campanha */
    std::vector<Treinador*> _adversarios; /**< Vetor de ponteiros para os adversários enfrentados na campanha */
    double _tempo_de_jogo; /**< Tempo de jogo acumulado na campanha */

public:
    /**
     * @brief Construtor da classe Campanha.
     */
    Campanha(){}

    /**
     * @brief Destrutor da classe Campanha.
     */
    ~Campanha(){}

    //Printers
    /**
     * @brief Imprime a quantidade de vitórias e derrotas da campanha.
     */
    void informar_quantidade_de_derrotas_e_vitorias(){}

    /**
     * @brief Imprime os adversários enfrentados na campanha.
     */
    void informar_adversarios(){}

    // Setters
    /**
     * @brief Atualiza o tempo de jogo acumulado na campanha.
     */
    void atualiza_tempo_de_jogo(){}

    /**
     * @brief Registra uma vitória na campanha.
     */
    void ganhou_batalha(){}

    /**
     * @brief Registra uma derrota na campanha.
     */
    void perdeu_batalha(){}
    
    // Getters
    /**
     * @brief Obtém a quantidade de batalhas realizadas na campanha.
     * @return A quantidade de batalhas.
     */
    int get_quantidade_batalhas() const{}

    /**
     * @brief Obtém a quantidade de derrotas sofridas na campanha.
     * @return A quantidade de derrotas.
     */
    int get_quantidade_derrotas() const{}

    /**
     * @brief Obtém o tempo de jogo acumulado na campanha.
     * @return O tempo de jogo.
     */
    double get_tempo_de_jogo() const{}

    /**
     * @brief Obtém o treinador da campanha.
     * @return O treinador.
     */
    Treinador* get_treinador() const{}

    /**
     * @brief Obtém os adversários enfrentados na campanha.
     * @return Um vetor de ponteiros para os adversários.
     */
    std::vector<Treinador*> get_adversarios() const{}
};

#endif // CAMPANHA_HPP