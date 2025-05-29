#ifndef MOCHILA_HPP
#define MOCHILA_HPP

#include "Item.hpp"
#include "Insignia.hpp"
#include <string>
//#include <Pokedex.hpp>

/**
 * @brief Classe que representa uma mochila.
 * 
 * Esta classe é responsável por armazenar os itens e insígnias do jogador.
 */
class Mochila{
    private:
    //Pokedex _pokedex;
    std::vector<Item*> _itens; /**< Vetor de ponteiros para os itens armazenados na mochila. */
    std::vector<Insignia*> _insignias; /**< Vetor de ponteiros para as insígnias armazenadas na mochila. */

    public:
    /**
     * @brief Construtor da classe Mochila.
     */
    Mochila();

    /**
     * @brief Destrutor da classe Mochila.
     */
    ~Mochila();

    /**
     * @brief Informa os itens presentes na mochila.
     */
    void informarItens();

    /**
     * @brief Informa as insígnias presentes na mochila.
     */
    void informarInsignias();

    /**
     * @brief Adiciona um item à mochila.
     * 
     * @param item Ponteiro para o item a ser adicionado.
     */
    void adicionarItem(Item* item);

    /**
     * @brief Adiciona uma insígnia à mochila.
     * 
     * @param insignia Ponteiro para a insígnia a ser adicionada.
     */
    void adicionarInsignia(Insignia* insignia);
};

#endif