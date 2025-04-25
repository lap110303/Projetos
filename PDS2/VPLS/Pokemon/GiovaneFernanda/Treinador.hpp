#ifndef TREINADOR_H
#define TREINADOR_H

#include <iostream>
#include <vector>
#include <string>

#include "Pokemon.hpp"
#include "PokemonEletrico.hpp"
#include "PokemonAquatico.hpp"
#include "PokemonExplosivo.hpp"

class Treinador {
private:
    std::vector<Pokemon*> _pokemons;

public:
    std::string _nome;

    Treinador(std::string nome);
    void cadastrar_pokemon_eletrico(std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio);
    void cadastrar_pokemon_aquatico(std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato);
    void cadastrar_pokemon_explosivo(std::string nome, std::string tipo_ataque, double forca_ataque, double temperatura_explosao);
    Pokemon* usar_pokemon(unsigned int idpk);
    void imprimir_informacoes();
};

#endif