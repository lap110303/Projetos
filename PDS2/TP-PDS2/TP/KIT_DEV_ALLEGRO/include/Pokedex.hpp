#ifndef POKEDEX_HPP
#define POKEDEX_HPP

#include <string>
#include <vector>
#include "Pokemon.hpp"

/**
 * @brief Classe que representa uma Pokedex.
 * 
 * A classe Pokedex é responsável por armazenar e gerenciar os Pokemons obtidos e existentes.
 */
class Pokedex {
private:
    std::vector<Pokemon*> _pokemons_obtidos; /**< Vetor de ponteiros para os Pokemons obtidos. */
    std::vector<Pokemon> _pokemons_existentes; /**< Vetor de Pokemons existentes. */

public:
    /**
     * @brief Construtor da classe Pokedex.
     */
    Pokedex();

    /**
     * @brief Adiciona um Pokemon capturado à Pokedex.
     * 
     * @param pokemon Ponteiro para o Pokemon capturado.
     */
    void pokemon_capturado(Pokemon* pokemon);

    //adcionar_pokemon_existente();

    /**
     * @brief Retorna uma lista dos Pokemons obtidos.
     * 
     * @return Vetor de Pokemons obtidos.
     */
    std::vector<Pokemon> listar_pokemons_obtidos();

    /**
     * @brief Retorna uma lista dos Pokemons existentes.
     * 
     * @return Vetor de Pokemons existentes.
     */
    std::vector<Pokemon> listar_pokemons_existentes();

    /**
     * @brief Imprime os dados da Pokedex.
     */
    void imprimirDados();

};

#endif
