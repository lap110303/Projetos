#ifndef POKEMON_ELETRICO_HPP
#define POKEMON_ELETRICO_HPP

#include "Pokemon.hpp"

/**
 * @file PokemonEletrico.hpp
 * @brief Definição da classe PokemonEletrico.
 */

/**
 * @class PokemonEletrico
 * @brief Classe que representa um Pokémon do tipo Elétrico.
 * @details A classe PokemonEletrico herda da classe Pokemon e adiciona atributos e métodos específicos para um Pokémon do tipo Elétrico.
 */

class PokemonEletrico : public Pokemon {
    private:
    std::string _ataque_basico; /**< Nome do ataque básico do Pokémon Elétrico. */
    double _dano_ataque_basico; /**< Dano causado pelo ataque básico do Pokémon Elétrico. */
    double _dano_ataque_eletrico; /**< Dano causado pelo ataque elétrico do Pokémon Elétrico. */
    std::string _ataque_eletrico; /**< Nome do ataque elétrico do Pokémon Elétrico. */

    public:
    /**
     * @brief Construtor da classe PokemonEletrico.
     * @param nome Nome do Pokémon.
     * @param level Nível do Pokémon.
     * @param dano_ataque Dano causado pelo Pokémon.
     * @param energia Energia do Pokémon.
     * @param defesa Defesa do Pokémon.
     * @param ataque_basico Nome do ataque básico do Pokémon.
     * @param ataque_eletrico Nome do ataque elétrico do Pokémon.
     */
    PokemonEletrico(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_eletrico);

    /**
     * @brief Destrutor da classe PokemonEletrico.
     */
    ~PokemonEletrico();

    /**
     * @brief Obtém o nome do ataque básico do Pokémon Elétrico.
     * @return Nome do ataque básico.
     */
    std::string getAtaqueBasico();

    /**
     * @brief Obtém o nome do ataque elétrico do Pokémon Elétrico.
     * @return Nome do ataque elétrico.
     */
    std::string getAtaqueEletrico();

    /**
     * @brief Obtém o dano causado pelo ataque básico do Pokémon Elétrico.
     * @return Dano do ataque básico.
     */
    double getDanoBasico();

    /**
     * @brief Obtém o dano causado pelo ataque elétrico do Pokémon Elétrico.
     * @return Dano do ataque elétrico.
     */
    double getDanoEletrico();

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