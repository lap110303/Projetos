#include "PokemonAgua.hpp"


PokemonAgua::PokemonAgua(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_agua): Pokemon(nome, "agua", level, dano_ataque, energia, defesa, "fisico"), _ataque_basico(ataque_basico), _ataque_agua(ataque_agua) {}

PokemonAgua::~PokemonAgua() {}

std::string PokemonAgua::getAtaqueBasico() {
    return this->_ataque_basico;
}

std::string PokemonAgua::getAtaqueAgua() {
    return this->_ataque_agua;
}

void PokemonAgua::receberDano(double dano, std::string tipo) {
    double energia = this->getEnergia();
    if (tipo == this->getFraqueza()) {
        energia -= dano * 2;
    } else {
        energia -= dano;
    }
    this->setEnergia(energia);
}

double PokemonAgua::calcularDanoAtaque(std::string ataque) {
    if (ataque == this->getAtaqueBasico()) {
        return this->getDano() * this->getDanoBasico();
    } else {
        return this->getDano() * this->getDanoAgua();
    }
}

double PokemonAgua::getDanoBasico() {
    return 2.0; // Ajuste conforme necessário
}

double PokemonAgua::getDanoAgua() {
    return 8.0; // Ajuste conforme necessário
}
