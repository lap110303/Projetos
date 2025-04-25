#include "Treinador.hpp"

Treinador::Treinador(string nome): _nome(nome) {}

Treinador::~Treinador() {
    for (Pokemon* pokemon : _pokemons) {
        delete pokemon;
    }
    _pokemons.clear();
}


void Treinador:: cadastrar_pokemon_eletrico(string nome, string tipo_ataque, double forca_ataque, double potencia_raio) {
    PokemonEletrico* novoPokemonEletrico = new PokemonEletrico(nome, tipo_ataque, forca_ataque, potencia_raio);
    _pokemons.push_back(novoPokemonEletrico);
}

void Treinador::cadastrar_pokemon_aquatico(string nome, string tipo_ataque, double forca_ataque, double litros_jato) {
    PokemonAquatico* novoPokemonAquatico = new PokemonAquatico(nome, tipo_ataque, forca_ataque, litros_jato);
    _pokemons.push_back(novoPokemonAquatico);
}

void Treinador::cadastrar_pokemon_explosivo(string nome, string tipo_ataque, double forca_ataque, double temperatura_explosao) {
    PokemonExplosivo* novoPokemonExplosivo = new PokemonExplosivo(nome, tipo_ataque, forca_ataque, temperatura_explosao);
    _pokemons.push_back(novoPokemonExplosivo);
}


void Treinador::imprimir_informacoes(){
    cout << "Nome: "<<_nome << endl << "----------" << endl;
    if(_pokemons.empty()){
        cout << "Nenhum Pokemon cadastrado!" << endl << "----------" << endl;
    }
    else{
        for (size_t i = 0; i < _pokemons.size(); i++) {
            _pokemons[i]->imprimir_informacoes();
            cout << "----------" << endl;
        }
    }
}

void Treinador::adicionar_pokemon(Pokemon* novoPokemon) {
    _pokemons.push_back(novoPokemon);
}

void Treinador::falar_nome(){
    cout << _nome;
}