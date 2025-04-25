#ifndef POKEMON_EXPLOSIVO_HPP
#define POKEMON_EXPLOSIVO_HPP

#include "pokemon.hpp"

class PokemonExplosivo : public Pokemon {
    double _temperatura_explosao;

public:
    PokemonExplosivo(string nome, string tipo_ataque, double forca_ataque, double temperatura_explosao);
    void falar_tipo_ataque();
    double calcular_dano() override;
    double ataque_explosivo();
};

#endif
