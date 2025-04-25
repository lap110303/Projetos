#ifndef PERSONAGEM_H
#define PERSONAGEM_H

#include <iostream>

class Personagem {  
    protected:
        std::string nome;
        std::string universo;

    public:
        /**
         * @brief Constroi um personagem
         * @param _nome Nome do personagem
         * @param _universo Universo ao qual o personagem pertence
         */
        Personagem(std::string _nome, std::string _universo);

        /**
         * @brief Destrói um personagem
         */
        virtual ~Personagem();

        /**
         * @brief Retorna nome do personagem
         */
        std::string getNome();
        /**
         * @brief Define o nome do personagem
         * @param _nome Universo ao qual o personagem pertence
         */
        void setNome(std::string _nome);

        /**
         * @brief Da a introdução do personagem
         */
        virtual void introducao() = 0;
};

#endif