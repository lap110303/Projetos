#ifndef ITEM_HPP
#define ITEM_HPP

#include <vector>
#include "Pokemon.hpp"

/**
 * @brief Classe que representa um item no jogo.
 * 
 * Esta classe define as propriedades e métodos de um item no jogo.
 * Um item possui um nome, um modificador de energia, um modificador de ataque
 * e um modificador de defesa. Ele pode ser utilizado para modificar o status
 * de um Pokémon, como a energia, o dano de ataque e a defesa.
 */
class Item {
    private:
    double modificador_energia; /**< Modificador de energia do item. */
    double modificador_ataque; /**< Modificador de ataque do item. */
    double modificador_defesa; /**< Modificador de defesa do item. */
    std::string _nome; /**< Nome do item. */
    
    public:
    /**
     * @brief Construtor da classe Item.
     * 
     * @param nome O nome do item.
     * @param energia O modificador de energia do item.
     * @param ataque O modificador de ataque do item.
     * @param defesa O modificador de defesa do item.
     */
    Item(std::string nome, double energia, double ataque, double defesa);

    /**
     * @brief Destrutor da classe Item.
     */
    ~Item();

    /**
     * @brief Obtém o nome do item.
     * 
     * @return O nome do item.
     */
    std::string getNome();

    /**
     * @brief Obtém o modificador de energia do item.
     * 
     * @return O modificador de energia do item.
     */
    double getEnergia();

    /**
     * @brief Modifica o status de um Pokémon.
     */
    void modificarStatus();

    /**
     * @brief Modifica a energia de um Pokémon.
     * 
     * @param pokemon O Pokémon a ser modificado.
     * @param energia A quantidade de energia a ser modificada.
     */
    void modificarEnergia(Pokemon* pokemon, double energia);

    /**
     * @brief Modifica o dano de ataque de um Pokémon.
     * 
     * @param pokemon O Pokémon a ser modificado.
     * @param ataque O valor do modificador de ataque.
     */
    void modificarDanoAtaque(Pokemon* pokemon, double ataque);

    /**
     * @brief Modifica a defesa de um Pokémon.
     * 
     * @param pokemon O Pokémon a ser modificado.
     * @param defesa O valor do modificador de defesa.
     */
    void modificarDefesa(Pokemon* pokemon, double defesa);
};

#endif