#ifndef POKEMON_NORMAL_HPP
#define POKEMON_NORMAL_HPP

#include "Pokemon.hpp"

/**
 * @class PokemonNormal
 * @brief Classe que representa um Pokémon do tipo Normal.
 * 
 * Esta classe herda da classe base Pokemon e adiciona atributos e métodos específicos para um Pokémon do tipo Normal.
 */
class PokemonNormal : public Pokemon {
    private:
    std::string _ataque_basico;
    double _dano_ataque_basico;
    double _dano_ataque_normal;
    std::string _ataque_normal;

    public:
    /**
     * @brief Construtor da classe PokemonNormal.
     * @param nome O nome do Pokémon.
     * @param level O nível do Pokémon.
     * @param dano_ataque O dano de ataque do Pokémon.
     * @param energia A energia do Pokémon.
     * @param defesa A defesa do Pokémon.
     * @param ataque_basico O nome do ataque básico do Pokémon.
     * @param ataque_normal O nome do ataque normal do Pokémon.
     */
    PokemonNormal(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_normal);

    /**
     * @brief Destrutor da classe PokemonNormal.
     */
    ~PokemonNormal();

    /**
     * @brief Obtém o nome do ataque básico do Pokémon.
     * @return O nome do ataque básico.
     */
    std::string getAtaqueBasico();

    /**
     * @brief Obtém o nome do ataque normal do Pokémon.
     * @return O nome do ataque normal.
     */
    std::string getAtaqueNormal();

    /**
     * @brief Obtém o dano do ataque básico do Pokémon.
     * @return O dano do ataque básico.
     */
    double getDanoBasico();

    /**
     * @brief Obtém o dano do ataque normal do Pokémon.
     * @return O dano do ataque normal.
     */
    double getDanoNormal();

    /**
     * @brief Recebe dano de um ataque.
     * @param dano O valor do dano recebido.
     * @param tipo O tipo de ataque recebido.
     */
    void receberDano(double dano, std::string tipo) override;

    /**
     * @brief Calcula o dano de um ataque específico.
     * @param ataque O nome do ataque a ser calculado.
     * @return O dano do ataque.
     */
    double calcularDanoAtaque(std::string ataque) override;
};

#endif // POKEMON_NORMAL_HPP
