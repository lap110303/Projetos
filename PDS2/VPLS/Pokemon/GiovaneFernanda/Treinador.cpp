#include "Treinador.hpp"
#include <iostream>

Treinador::Treinador(std::string nome){
    this->_nome = nome;
}

void Treinador::cadastrar_pokemon_eletrico(std::string nome, std::string tipo_ataque, double forca_ataque, double potencia_raio) {
    PokemonEletrico* pokemon = new PokemonEletrico(nome, tipo_ataque, forca_ataque, potencia_raio);
    _pokemons.push_back(pokemon);
}

void Treinador::cadastrar_pokemon_aquatico(std::string nome, std::string tipo_ataque, double forca_ataque, double litros_jato) {
    PokemonAquatico* pokemon = new PokemonAquatico(nome, tipo_ataque, forca_ataque, litros_jato);
    _pokemons.push_back(pokemon);
}

void Treinador::cadastrar_pokemon_explosivo(std::string nome, std::string tipo_ataque, double forca_ataque, double temperatura_explosao) {
    PokemonExplosivo* pokemon = new PokemonExplosivo(nome, tipo_ataque, forca_ataque, temperatura_explosao);
    _pokemons.push_back(pokemon);
}

Pokemon* Treinador::usar_pokemon(unsigned int idpk) {
    if (idpk >= 0 && idpk < _pokemons.size()) {
        return _pokemons[idpk];
    }
    return nullptr;
}

void Treinador::imprimir_informacoes() {
    std::cout << "Nome: " << _nome << std::endl;
    std::cout << "----------" << std::endl;
    if (_pokemons.empty()) {
        std::cout << "Nenhum Pokemon cadastrado!" << std::endl;
        std::cout << "----------" << std::endl;
    } else {
        for (unsigned int i = 0; i < _pokemons.size(); ++i) {
            //std::cout << "(Informacoes Pokemon " << i << ")" << std::endl;
            _pokemons[i]->imprimir_informacoes();
            std::cout << "----------" << std::endl;
        }
    }
}