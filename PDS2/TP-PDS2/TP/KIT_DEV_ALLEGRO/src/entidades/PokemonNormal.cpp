#include "PokemonNormal.hpp"

PokemonNormal::PokemonNormal(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_normal): Pokemon(nome, "normal", level, dano_ataque, energia, defesa, "fisico"), _ataque_basico(ataque_basico), _ataque_normal(ataque_normal) {}

PokemonNormal::~PokemonNormal() {}

std::string PokemonNormal::getAtaqueBasico() {
    return this->_ataque_basico;
}

std::string PokemonNormal::getAtaqueNormal() {
    return this->_ataque_normal;
}

void PokemonNormal::receberDano(double dano, std::string tipo) {
    double energia = this->getEnergia();
    if (tipo == this->getFraqueza()) {
        energia -= dano * 2;
    } else {
        energia -= dano;
    }
    this->setEnergia(energia);
}

double PokemonNormal::calcularDanoAtaque(std::string ataque) {
    if (ataque == this->getAtaqueBasico()) {
        return this->getDano() * this->getDanoBasico();
    } else {
        return this->getDano() * this->getDanoNormal();
    }
}

double PokemonNormal::getDanoBasico() {
    return 1.5; // Ajuste conforme necessário
}

double PokemonNormal::getDanoNormal() {
    return 5.0; // Ajuste conforme necessário
}
