#include "PokemonExplosivo.hpp"
#include <iostream>

using namespace std;

PokemonExplosivo::PokemonExplosivo(string nome, string tipo_ataque, double forca_ataque, double temperatura_explosao)
    : Pokemon(nome, tipo_ataque, forca_ataque), _temperatura_explosao(temperatura_explosao) {}

void PokemonExplosivo::falar_tipo_ataque() {
    Pokemon::falar_tipo_ataque();
    cout << "Boom!" << endl;
}

double PokemonExplosivo::calcular_dano() {
    return getForcaAtaque() / _temperatura_explosao;
}

double PokemonExplosivo::ataque_explosivo() {
    return getForcaAtaque() / _temperatura_explosao;
}
