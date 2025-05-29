#ifndef POKEMON_HPP
#define POKEMON_HPP

#include <iostream>
#include <string>

/**
 * @brief Classe que representa um Pokémon.
 * 
 * Essa classe representa um Pokémon com seus atributos e métodos.
 */
class Pokemon {
private:
    std::string _nome; /**< Nome do Pokémon */
    std::string _tipo; /**< Tipo do Pokémon */
    int _level; /**< Nível do Pokémon */
    double _dano_ataque; /**< Dano de ataque do Pokémon */
    double _dano_sofrido; /**< Dano sofrido pelo Pokémon */
    double _energia; /**< Energia do Pokémon */
    double _defesa; /**< Valor da defesa do Pokémon */
    std::string _fraqueza; /**< Fraqueza do Pokémon */

public:
    /**
     * @brief Construtor da classe Pokemon.
     * 
     * @param nome Nome do Pokémon.
     * @param tipo Tipo do Pokémon.
     * @param level Nível do Pokémon.
     * @param dano_ataque Dano de ataque do Pokémon.
     * @param energia Energia do Pokémon.
     * @param defesa Valor da defesa do Pokémon.
     * @param fraqueza Fraqueza do Pokémon.
     */
    Pokemon(std::string nome, std::string tipo, int level, double dano_ataque, double energia, double defesa, std::string fraqueza);

    /**
     * @brief Destrutor da classe Pokemon.
     */
    ~Pokemon();

    /**
     * @brief Obtém o nome do Pokémon.
     * 
     * @return Nome do Pokémon.
     */
    std::string getNome();

    /**
     * @brief Obtém o tipo do Pokémon.
     * 
     * @return Tipo do Pokémon.
     */
    std::string getTipo();

    /**
     * @brief Obtém o dano de ataque do Pokémon.
     * 
     * @return Dano de ataque do Pokémon.
     */
    double getDano();

    /**
     * @brief Define o dano de ataque do Pokémon.
     * 
     * @param dano_ataque Novo valor do dano de ataque do Pokémon.
     */
    void setDano(double dano_ataque);

    /**
     * @brief Obtém o dano sofrido pelo Pokémon.
     * 
     * @return Dano sofrido pelo Pokémon.
     */
    double getDanoSofrido();

    /**
     * @brief Define o dano sofrido pelo Pokémon.
     * 
     * @param dano_sofrido Novo valor do dano sofrido pelo Pokémon.
     */
    void setDanoSofrido(double dano_sofrido);

    /**
     * @brief Obtém a fraqueza do Pokémon.
     * 
     * @return Fraqueza do Pokémon.
     */
    std::string getFraqueza();

    /**
     * @brief Obtém a energia do Pokémon.
     * 
     * @return Energia do Pokémon.
     */
    double getEnergia();

    /**
     * @brief Retorna uma string com informações do Pokémon.
     * 
     * @return String com informações do Pokémon.
     */
    std::string imprimirInfos();

    /**
     * @brief Define a energia do Pokémon.
     * 
     * @param energia Nova energia do Pokémon.
     */
    void setEnergia(double energia);

    /**
     * @brief Realiza um ataque a um Pokémon defensor.
     * 
     * @param defensor Pokémon que será atacado.
     * @param ataque Tipo de ataque a ser realizado.
     */
    void atacar(Pokemon* defensor, std::string ataque);

    /**
     * @brief Função virtual pura para receber dano.
     * 
     * 
     * @param dano Valor do dano recebido.
     * @param tipo Tipo do ataque que causou o dano.
     */
    virtual void receberDano(double dano, std::string tipo) = 0;

    /**
     * @brief Função virtual pura para calcular dano de ataque.
     * 
     * @param ataque Tipo de ataque a ser calculado.
     * @return Valor do dano causado pelo ataque.
     */
    virtual double calcularDanoAtaque(std::string ataque) = 0;
};

#endif
