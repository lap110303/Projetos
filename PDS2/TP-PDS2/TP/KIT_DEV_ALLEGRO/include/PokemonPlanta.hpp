#ifndef POKEMON_PLANTA_HPP
#define POKEMON_PLANTA_HPP

#include "Pokemon.hpp"

/**
 * @file PokemonPlanta.hpp
 * @brief Definição da classe PokemonPlanta, que representa um Pokémon do tipo planta.
 */

/**
 * @class PokemonPlanta
 * @brief Classe que representa um Pokémon do tipo planta.
 * @details A classe PokemonPlanta herda da classe base Pokemon e adiciona atributos e métodos específicos para um Pokémon do tipo planta.
 */
class PokemonPlanta : public Pokemon {
private:
    std::string _ataque_basico; /**< Nome do ataque básico do Pokémon do tipo planta. */
    double _dano_ataque_basico; /**< Dano causado pelo ataque básico do Pokémon do tipo planta. */
    double _dano_ataque_planta; /**< Dano causado pelo ataque específico de planta do Pokémon do tipo planta. */
    std::string _ataque_planta; /**< Nome do ataque específico de planta do Pokémon do tipo planta. */
    // Adicionar fraquezas ou atributos específicos de um Pokémon do tipo planta, se necessário

public:
    /**
     * @brief Construtor da classe PokemonPlanta.
     * @param nome Nome do Pokémon.
     * @param level Nível do Pokémon.
     * @param dano_ataque Dano base do Pokémon.
     * @param energia Energia do Pokémon.
     * @param defesa Defesa do Pokémon.
     * @param ataque_basico Nome do ataque básico do Pokémon.
     * @param ataque_planta Nome do ataque específico de planta do Pokémon.
     */
    PokemonPlanta(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_planta);

    /**
     * @brief Destrutor da classe PokemonPlanta.
     */
    ~PokemonPlanta();

    /**
     * @brief Obtém o nome do ataque básico do Pokémon do tipo planta.
     * @return Nome do ataque básico.
     */
    std::string getAtaqueBasico();

    /**
     * @brief Obtém o nome do ataque específico de planta do Pokémon do tipo planta.
     * @return Nome do ataque específico de planta.
     */
    std::string getAtaquePlanta();

    /**
     * @brief Obtém o dano causado pelo ataque básico do Pokémon do tipo planta.
     * @return Dano do ataque básico.
     */
    double getDanoBasico();

    /**
     * @brief Obtém o dano causado pelo ataque específico de planta do Pokémon do tipo planta.
     * @return Dano do ataque específico de planta.
     */
    double getDanoPlanta();

    /**
     * @brief Recebe dano de um ataque.
     * @param dano Dano recebido.
     * @param tipo Tipo do ataque.
     */
    void receberDano(double dano, std::string tipo) override;

    /**
     * @brief Calcula o dano causado por um ataque.
     * @param ataque Nome do ataque.
     * @return Dano causado pelo ataque.
     */
    double calcularDanoAtaque(std::string ataque) override;
};

#endif
