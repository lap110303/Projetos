#include "PokemonEletrico.hpp"
#include <iostream>

PokemonEletrico::PokemonEletrico(std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio)
    : Pokemon(nome, tipo_ataque, forca_ataque), _potencia_raio(potencia_raio) {}

void PokemonEletrico::falar_tipo_ataque() {
    std::cout << _tipo_ataque << "!" << std::endl;
    std::cout << "Bzzzz!" << std::endl;
}

double PokemonEletrico::calcular_dano() {
    return ataque_eletrico();
}

double PokemonEletrico::ataque_eletrico() {
    return _forca_ataque * _potencia_raio;
}