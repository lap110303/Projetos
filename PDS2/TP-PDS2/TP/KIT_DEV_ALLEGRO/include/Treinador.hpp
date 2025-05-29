#ifndef TREINADOR_HPP
#define TREINADOR_HPP

#include "Mochila.hpp"
#include <vector>

/**
 * @class Treinador
 * 
 * @brief Classe que representa um treinador.
 * 
 * Esta classe representa um treinador no jogo. Um treinador possui um nome, uma região,
 * uma mochila, e um status da mochila. Ele pode abrir e fechar a mochila, além de imprimir
 * suas informações.
 */
class Treinador{
    private:
    std::string _nome; /**< Nome do treinador */
    std::string _regiao; /**< Região do treinador */
    Mochila* _mochila; /**< Ponteiro para a mochila do treinador */
    std::string _status_mochila; /**< Status da mochila do treinador */

    public:
    /**
     * @brief Construtor da classe Treinador.
     * 
     * @param nome O nome do treinador.
     * @param regiao A região do treinador.
     */
    Treinador(std::string nome, std::string regiao);

    /**
     * @brief Destrutor da classe Treinador.
     */
    ~Treinador();

    /**
     * @brief Obtém o nome do treinador.
     * 
     * @return O nome do treinador.
     */
    std::string getNome();

    /**
     * @brief Obtém a região do treinador.
     * 
     * @return A região do treinador.
     */
    std::string getRegiao();

    /**
     * @brief Abre a mochila do treinador.
     */
    void abrirMochila();

    /**
     * @brief Fecha a mochila do treinador.
     */
    void fecharMochila();

    /**
     * @brief Imprime as informações do treinador.
     * 
     * @return Uma string contendo as informações do treinador.
     */
    std::string imprimirInfos();
};

#endif