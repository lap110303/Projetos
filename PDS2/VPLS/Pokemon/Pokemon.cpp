#include "Pokemon.hpp"

#include <iostream>
#include <vector>
#include <string>

Pokemon::Pokemon(std::string nome, std::string tipo_ataque, double forca_ataque){
    this->_nome = nome;
    this->_tipo_ataque = tipo_ataque;
    this->_forca_ataque = forca_ataque;
    this->_energia = 100;
}

void Pokemon::falar_nome() {
    std::cout << _nome << "!" << std::endl;
}

void Pokemon::falar_tipo_ataque() {
    std::cout << _tipo_ataque << "!" << std::endl;
}

void Pokemon::imprimir_status() {
    std::cout << _nome << "!" << std::endl;
    std::cout << "Energia: " << _energia << std::endl;
}

void Pokemon::imprimir_informacoes() {
    std::cout << "Pokemon: " << _nome << std::endl;
    std::cout << "Tipo ataque: " << _tipo_ataque << std::endl;
    std::cout << "Dano: " << calcular_dano() << std::endl;
    std::cout << "Energia: " << _energia << std::endl;
}

void Pokemon::atacar(Pokemon* outro_pokemon) {
    falar_nome();
    falar_tipo_ataque();
    double dano = calcular_dano();
    std::cout << "Dano: " << dano << std::endl;
    outro_pokemon->receber_dano(dano);
}

void Pokemon::receber_dano(double valor_dano) {
    falar_nome();
    _energia -= valor_dano;
    std::cout << "Energia: " << _energia << std::endl;
    if (_energia <= 0) {
        std::cout << _nome << " morreu!" << std::endl;
        _energia = 0;
    }
}