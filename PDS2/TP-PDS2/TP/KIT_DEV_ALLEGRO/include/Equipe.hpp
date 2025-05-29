#ifndef EQUIPE_HPP
#define EQUIPE_HPP

#include <iostream>
#include <vector>
#include <Pokemon.hpp>

/**
 * @brief Classe que representa uma equipe de Pokémon.
 * 
 * Esta classe é responsável por gerenciar uma equipe de Pokémon, permitindo adicionar, remover,
 * imprimir e trocar o Pokémon ativo da equipe.
 */
class Equipe{
    private:
    std::vector<Pokemon*> time; /**< Vetor de ponteiros para Pokémon que representa a equipe. */
    std::string _pokemon_ativo; /**< Nome do Pokémon ativo na equipe. */

    public:
    /**
     * @brief Construtor padrão da classe Equipe.
     */
    Equipe(){}

    /**
     * @brief Destrutor da classe Equipe.
     */
    ~Equipe(){}

    /**
     * @brief Adiciona um Pokémon à equipe.
     */
    void adicionar_pokemon(Pokemon* pokemon);

    /**
     * @brief Remove um Pokémon da equipe.
     */
    void remover_pokemon(Pokemon* pokemon);

    /**
     * @brief Imprime os Pokémon da equipe.
     */
    void imprimir_time();

    /**
     * @brief Troca o Pokémon ativo da equipe.
     */
    void trocar_pokemon_ativo(Pokemon* pokemon);

};

#endif