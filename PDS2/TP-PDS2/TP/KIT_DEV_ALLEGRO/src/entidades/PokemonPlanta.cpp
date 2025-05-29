#include "PokemonPlanta.hpp"

PokemonPlanta::PokemonPlanta(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_planta): Pokemon(nome, "planta", level, dano_ataque, energia, defesa, "fisico"), _ataque_basico(ataque_basico), _ataque_planta(ataque_planta) {}

PokemonPlanta::~PokemonPlanta() {}

std::string PokemonPlanta::getAtaqueBasico() {
    return this->_ataque_basico;
}

std::string PokemonPlanta::getAtaquePlanta() {
    return this->_ataque_planta;
}

void PokemonPlanta::receberDano(double dano, std::string tipo) {
    double energia = this->getEnergia();
    if (tipo == this->getFraqueza()) {
        // Considerando um multiplicador de dano diferente para fraqueza
        energia -= dano * 1.5;
    } else {
        energia -= dano;
    }
    this->setEnergia(energia);
}

double PokemonPlanta::calcularDanoAtaque(std::string ataque) {
    if (ataque == this->getAtaqueBasico()) {
        // Mudar para _dano_basico quando definido
        return this->getDano() * this->getDanoBasico();
    } else {
        // Mudar para _dano_planta quando definido
        return this->getDano() * this->getDanoPlanta();
    }
}

double PokemonPlanta::getDanoBasico() {
    return 3.5;
}

double PokemonPlanta::getDanoPlanta() {
    return 9.0;
}
