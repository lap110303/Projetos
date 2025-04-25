#include "PokemonAquatico.hpp"
#include <iostream>

using namespace std;

PokemonAquatico::PokemonAquatico(string nome, string tipo_ataque, double forca_ataque, double litros_jato)
    : Pokemon(nome, tipo_ataque, forca_ataque), _litros_jato(litros_jato) {}

void PokemonAquatico::falar_tipo_ataque() {
    Pokemon::falar_tipo_ataque();
    cout << "Splash!" << endl;
}

double PokemonAquatico::calcular_dano() {
    return getForcaAtaque() + _litros_jato;
}

double PokemonAquatico::ataque_aquatico() {
    return getForcaAtaque() + _litros_jato;
}
