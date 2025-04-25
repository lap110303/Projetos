#ifndef POKEMON_ELETRICO_HPP
#define POKEMON_ELETRICO_HPP

#include "pokemon.hpp"

class PokemonEletrico : public Pokemon {
    double _potencia_raio;

public:
    PokemonEletrico(string nome, string tipo_ataque, double forca_ataque, double potencia_raio);
    void falar_tipo_ataque();
    double calcular_dano() override;
    double ataque_eletrico();
};

#endif
