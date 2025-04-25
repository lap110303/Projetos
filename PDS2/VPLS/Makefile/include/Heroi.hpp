#ifndef HEROI_H
#define HEROI_H

#include "Vilao.hpp"
#include "Personagem.hpp"
#include <iostream>

class Heroi: public Personagem {
    protected:
        Vilao *inimigo;

    public:
        /**
         * @brief Construtor para o herói
         * @param _nome Nome do herói
         * @param _universo Universo ao qual o herói pertence
         * 
         */
        Heroi(std::string _nome, std::string _universo);
        /**
         * @brief Destrói o herói
         */
        virtual ~Heroi();

        /**
         * @brief Define o inimigo do herói
         * @param v Vilão que é inimigo do herói
         */
        void setInimigo(Vilao *v);

        /**
         * @brief Retorna o inimigo do herói
         */
        Vilao getInimigo();

        /**
         * @brief Dá uma breve introdução do herói
         */
        void introducao() override final;
};

#endif