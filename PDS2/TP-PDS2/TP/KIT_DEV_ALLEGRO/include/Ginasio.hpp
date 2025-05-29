#ifndef GINASIO_HPP
#define GINASIO_HPP

#include <string>
#include "Insignia.hpp"

/**
 * @brief Classe que representa um ginásio de Pokémon.
 * 
 * Esta classe armazena informações sobre um ginásio, como nome, treinador, região, elemento e insígnia.
 */
class Ginasio {
private:
    std::string _nome; ///< Nome do ginásio.
    std::string _treinador; ///< Nome do treinador do ginásio.
    std::string _regiao; ///< Região em que o ginásio está localizado.
    std::string _elemento; ///< Elemento associado ao ginásio.
    Insignia* _insignia; ///< Ponteiro para a insígnia do ginásio.

public:
    /**
     * @brief Construtor da classe Ginasio.
     * 
     * @param nome Nome do ginásio.
     * @param treinador Nome do treinador do ginásio.
     * @param regiao Região em que o ginásio está localizado.
     * @param elemento Elemento associado ao ginásio.
     * @param insignia Ponteiro para a insígnia do ginásio.
     */
    Ginasio(std::string nome, std::string treinador, std::string regiao, std::string elemento, std::string insignia){}

    /**
     * @brief Destrutor da classe Ginasio.
     */
    ~Ginasio(){}

    /**
     * @brief Obtém o nome do ginásio.
     * 
     * @return O nome do ginásio.
     */
    std::string get_nome() const{}

    /**
     * @brief Obtém o nome do treinador do ginásio.
     * 
     * @return O nome do treinador do ginásio.
     */
    std::string get_treinador() const{}

    /**
     * @brief Obtém a região em que o ginásio está localizado.
     * 
     * @return A região do ginásio.
     */
    std::string get_regiao() const{}

    /**
     * @brief Obtém o elemento associado ao ginásio.
     * 
     * @return O elemento do ginásio.
     */
    std::string get_elemento() const{}

    /**
     * @brief Obtém a insígnia do ginásio.
     * 
     * @return A insígnia do ginásio.
     */
    std::string get_insignia() const{}

    /**
     * @brief Informa o perfil do ginásio.
     * 
     * @param nome Nome do ginásio.
     * @param treinador Nome do treinador do ginásio.
     * @param regiao Região em que o ginásio está localizado.
     * @param elemento Elemento associado ao ginásio.
     * @param insignia Insígnia do ginásio.
     */
    void informar_perfil(const std::string& nome, const std::string& treinador, 
                         const std::string& regiao, const std::string& elemento, 
                         const std::string& insignia){}
};

#endif // GINASIO_HPP
