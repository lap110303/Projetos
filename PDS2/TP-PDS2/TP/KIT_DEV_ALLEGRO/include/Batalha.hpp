#ifndef BATALHA_HPP
#define BATALHA_HPP

#include <iostream>
#include <map>
#include "Equipe.hpp"
#include <string>

/**
 * @brief Classe que representa uma batalha entre equipes de Pokémon.
 * 
 * Esta classe gerencia uma batalha entre equipes de Pokémon, mantendo informações sobre as equipes,
 * o número de turnos, o vencedor e o perdedor.
 */
class Batalha {
private:
    std::map<std::string, Equipe*> equipes; /**< Mapa de equipes envolvidas na batalha */
    bool _turnos = true; /**< Número de turnos da batalha */
    std::string _vencedor; /**< Nome do vencedor da batalha */
    std::string _perdedor; /**< Nome do perdedor da batalha */

public:
    /**
     * @brief Construtor da classe Batalha.
     */
    Batalha();

    /**
     * @brief Destrutor da classe Batalha.
     */
    ~Batalha();

    /**
     * @brief Obtém as informações das equipes envolvidas na batalha.
     * 
     * @return String com as informações das equipes.
     */
    std::string getEquipes();

    /**
     * @brief Obtém o número de turnos da batalha.
     * 
     * @return Número de turnos da batalha.
     */
    bool getTurno();


    void setTurno();
    /**
     * @brief Obtém o nome do vencedor da batalha.
     * 
     * @return Nome do vencedor da batalha.
     */
    std::string getVencedor();

    /**
     * @brief Obtém o nome do perdedor da batalha.
     * 
     * @return Nome do perdedor da batalha.
     */
    std::string getPerdedor();

    /**
     * @brief Imprime as informações da batalha.
     * 
     * Imprime informações sobre as equipes, o número de turnos, o vencedor e o perdedor da batalha.
     */
    void imprimir_infos();
};

#endif
