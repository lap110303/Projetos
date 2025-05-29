#ifndef INSIGNIA_HPP
#define INSIGNIA_HPP

#include <iostream>
#include <string>

/**
 * @brief Classe que representa uma insígnia.
 * 
 * Esta classe possui atributos para armazenar o nome, tipo e existência de uma insígnia.
 */
class Insignia{
    private:
    std::string _nome; /**< Nome da insígnia */
    std::string _tipo; /**< Tipo da insígnia */
    bool _existe; /**< Indica se a insígnia existe ou não */

    
    public:
    /**
     * @brief Construtor da classe Insignia.
     * 
     * @param nome O nome da insígnia.
     * @param tipo O tipo da insígnia.
     */
    Insignia(std::string nome, std::string tipo);

    /**
     * @brief Destrutor da classe Insignia.
     */
    ~Insignia();

    /**
     * @brief Obtém o nome da insígnia.
     * 
     * @return O nome da insígnia.
     */
    std::string getNome();

    /**
     * @brief Obtém o tipo da insígnia.
     * 
     * @return O tipo da insígnia.
     */
    std::string getTipo();

    /**
     * @brief Verifica se a insígnia existe.
     * 
     * @return true se a insígnia existe, false caso contrário.
     */
    bool getExiste();

    /**
     * @brief Define a existência da insígnia.
     */
    void setExiste();
};

#endif