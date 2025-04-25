#ifndef POKEMONE_H
#define POKEMONE_H

#include "Pokemon.hpp"

class PokemonEletrico : public Pokemon {
private:
    double _potencia_raio;

public:
    PokemonEletrico(std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio);
    void falar_tipo_ataque();
    double calcular_dano();
    double ataque_eletrico();
};

#endif