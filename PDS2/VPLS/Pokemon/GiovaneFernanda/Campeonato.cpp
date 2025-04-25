#include "Campeonato.hpp"
#include <iostream>

void Campeonato::cadastrar_treinador(std::string nome) {
    Treinador* treinador = new Treinador(nome);
    _treinadores.push_back(treinador);
}

void Campeonato::cadastrar_pokemon_eletrico(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio) {
    if (idt >= 0 && idt < _treinadores.size()) {
        _treinadores[idt]->cadastrar_pokemon_eletrico(nome, tipo_ataque, forca_ataque, potencia_raio);
    }
}

void Campeonato::cadastrar_pokemon_aquatico(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato) {
    if (idt >= 0 && idt < _treinadores.size()) {
        _treinadores[idt]->cadastrar_pokemon_aquatico(nome, tipo_ataque, forca_ataque, litros_jato);
    }
}

void Campeonato::cadastrar_pokemon_explosivo(unsigned int idt, std::string nome, std::string tipo_ataque, double forca_ataque, double temperatura_explosao) {
    if (idt >= 0 && idt < _treinadores.size()) {
        _treinadores[idt]->cadastrar_pokemon_explosivo(nome, tipo_ataque, forca_ataque, temperatura_explosao);
    }
}

void Campeonato::imprimir_informacoes_treinador(unsigned int idt) {
    if (idt >= 0 && idt < _treinadores.size()) {
        _treinadores[idt]->imprimir_informacoes();
    }
}

void Campeonato::executar_batalha(unsigned int idt1, unsigned int idpk1, unsigned int idt2, unsigned int idpk2) {
    if (idt1 >= 0 && idt1 < _treinadores.size() && idt2 >= 0 && idt2 < _treinadores.size()) {
        Treinador* treinador1 = _treinadores[idt1];
        Treinador* treinador2 = _treinadores[idt2];

        Pokemon* pokemon1 = treinador1->usar_pokemon(idpk1);
        Pokemon* pokemon2 = treinador2->usar_pokemon(idpk2);

        if (pokemon1 != nullptr && pokemon2 != nullptr) {
            std::cout << "### Batalha ###" << std::endl;
            std::cout << treinador1->_nome << " (" << pokemon1->_nome << ") vs. "
                      << treinador2->_nome << " (" << pokemon2->_nome << ")" << std::endl;

            pokemon1->atacar(pokemon2);

            std::cout << "#########" << std::endl;
        }
    }
}