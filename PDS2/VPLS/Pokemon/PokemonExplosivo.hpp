#ifndef POKEMONEX_H
#define POKEMONEX_H

#include "Pokemon.hpp"

class PokemonExplosivo : public Pokemon {
private:
    double _temperatura_explosao;

public:
    PokemonExplosivo(std::string nome, std::string tipo_ataque, double forca_ataque, double temperatura_explosao);
    void falar_tipo_ataque();
    double calcular_dano();
    double ataque_explosivo();
};

#endif