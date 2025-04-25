#ifndef CAMPEONATO_H
#define CAMPEONATO_H

#include "Treinador.hpp"

#include <iostream>
#include <vector>
#include <string>

class Campeonato {
private:
    std::vector<Treinador*> _treinadores;

public:
    ~Campeonato();
    void cadastrar_treinador(std::string nome);
    void cadastrar_pokemon_eletrico(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio);
    void cadastrar_pokemon_aquatico(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato);
    void cadastrar_pokemon_explosivo(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double temperatura_explosao);
    void imprimir_informacoes_treinador(unsigned int idt);
    void executar_batalha(unsigned int idt1, unsigned int idpk1, unsigned int idt2, unsigned int idpk2);
};

#endif