#include "PokemonAquatico.hpp"
#include <iostream>

PokemonAquatico::PokemonAquatico(std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato)
    : Pokemon(nome, tipo_ataque, forca_ataque), _litros_jato(litros_jato) {}

void PokemonAquatico::falar_tipo_ataque() {
    std::cout << _tipo_ataque << "!" << std::endl;
    std::cout << "Splash!" << std::endl;
}

double PokemonAquatico::calcular_dano() {
    return ataque_aquatico();
}

double PokemonAquatico::ataque_aquatico() {
    return _forca_ataque + _litros_jato;
}