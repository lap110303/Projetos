#ifndef POKEMONA_H
#define POKEMONA_H

#include "Pokemon.hpp"

class PokemonAquatico : public Pokemon {
private:
    double _litros_jato;

public:
    PokemonAquatico(std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato);
    void falar_tipo_ataque();
    double calcular_dano();
    double ataque_aquatico();
};

#endif