#ifndef TREINADOR
#define TREINADOR
#include "PokemonEletrico.hpp"
#include "PokemonAquatico.hpp"
#include "PokemonExplosivo.hpp"
#include <vector>
#include <iostream>

class Treinador{
    string _nome;
    vector<Pokemon*> _pokemons;

    public:
    Treinador(string nome);
    ~Treinador();

    Pokemon* usar_pokemon(int idpk){
        return _pokemons[idpk];
    }

    void cadastrar_pokemon_eletrico(string nome, string tipo_ataque, double forca_ataque, double potencia_raio);
    void cadastrar_pokemon_aquatico(string nome, string tipo_ataque, double forca_ataque, double litros_jato);

    void cadastrar_pokemon_explosivo(string nome, string tipo_ataque, double forca_ataque, double temperatura_explosao);

    void imprimir_informacoes();

    void adicionar_pokemon(Pokemon* novoPokemon);

    void falar_nome();
};

#endif