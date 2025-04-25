#ifndef POKEMON_AQUATICO_HPP
#define POKEMON_AQUATICO_HPP

#include "pokemon.hpp"

class PokemonAquatico : public Pokemon {
    double _litros_jato;

public:
    PokemonAquatico(string nome, string tipo_ataque, double forca_ataque, double litros_jato);
    void falar_tipo_ataque();
    double calcular_dano() override;
    double ataque_aquatico();
};

#endif
