#ifndef VILAO_H
#define VILAO_H
#include <iostream>

#include "Personagem.hpp"

class Vilao: public Personagem {
    private:
        std::string objetivo;

    public:
        /**
         * @brief Construtor para o vilão
         * @param _nome Nome do vilão
         * @param _universo Universo ao qual o vilão pertence
         * @param _objetivo Objetivo do vilão
         * 
         */
        Vilao(std::string _nome, std::string _universo, std::string _objetivo);

        /**
         * @brief Destrói o vilão
         */
        virtual ~Vilao();

        /**
         * @brief Dá uma breve introdução do vilão
         */
        void introducao() override final;
};

#endif