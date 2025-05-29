#ifndef POKEMON_AGUA_HPP
#define POKEMON_AGUA_HPP

/**
 * @file PokemonAgua.hpp
 * @brief Definição da classe PokemonAgua.
 */

#include "Pokemon.hpp"

/**
 * @class PokemonAgua
 * @brief Classe que representa um Pokémon do tipo Água.
 */
class PokemonAgua : public Pokemon {
    private:
    std::string _ataque_basico; /**< Nome do ataque básico do Pokémon. */
    double _dano_ataque_basico; /**< Dano causado pelo ataque básico do Pokémon. */
    double _dano_ataque_agua; /**< Dano causado pelo ataque de água do Pokémon. */
    std::string _ataque_agua; /**< Nome do ataque de água do Pokémon. */

    public:
    /**
     * @brief Construtor da classe PokemonAgua.
     * @param nome Nome do Pokémon.
     * @param level Nível do Pokémon.
     * @param dano_ataque Dano causado pelo Pokémon.
     * @param energia Energia do Pokémon.
     * @param defesa Defesa do Pokémon.
     * @param ataque_basico Nome do ataque básico do Pokémon.
     * @param ataque_agua Nome do ataque de água do Pokémon.
     */
    PokemonAgua(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_agua);

    /**
     * @brief Destrutor da classe PokemonAgua.
     */
    ~PokemonAgua();

    /**
     * @brief Obtém o nome do ataque básico do Pokémon.
     * @return Nome do ataque básico.
     */
    std::string getAtaqueBasico();

    /**
     * @brief Obtém o nome do ataque de água do Pokémon.
     * @return Nome do ataque de água.
     */
    std::string getAtaqueAgua();

    /**
     * @brief Obtém o dano causado pelo ataque básico do Pokémon.
     * @return Dano do ataque básico.
     */
    double getDanoBasico();

    /**
     * @brief Obtém o dano causado pelo ataque de água do Pokémon.
     * @return Dano do ataque de água.
     */
    double getDanoAgua();

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

#endif // POKEMON_AGUA_HPP
