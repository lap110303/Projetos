#include "Campeonato.hpp"

Campeonato::~Campeonato() {
    for (Treinador* treinador : _treinadores) {
        delete treinador;
    }
    _treinadores.clear();
}


void Campeonato::cadastrar_treinador(string nome){
    Treinador* novoTreinador = new Treinador(nome);
    _treinadores.push_back(novoTreinador);
}

void Campeonato::cadastrar_pokemon_eletrico(int idt, string nome, string tipo_ataque, double forca_ataque, double potencia_raio){
    PokemonEletrico* novoPokemon = new PokemonEletrico(nome, tipo_ataque, forca_ataque, potencia_raio);
    _treinadores[idt]->adicionar_pokemon(novoPokemon);
}

void Campeonato::cadastrar_pokemon_aquatico(int idt, string nome, string tipo_ataque, double forca_ataque, double litros_jato){
     PokemonAquatico* novoPokemonAquatico = new PokemonAquatico(nome, tipo_ataque, forca_ataque, litros_jato);
     _treinadores[idt]->adicionar_pokemon(novoPokemonAquatico);
}

void Campeonato::cadastrar_pokemon_explosivo(int idt, string nome, string tipo_ataque, double forca_ataque, double temperatura_explosao){
     PokemonExplosivo* novoPokemonExplosivo = new PokemonExplosivo(nome, tipo_ataque, forca_ataque, temperatura_explosao);
    _treinadores[idt]->adicionar_pokemon(novoPokemonExplosivo);
}

void Campeonato::imprimir_informacoes_treinador(int idt){
    _treinadores[idt]->imprimir_informacoes();
}

void Campeonato::executar_batalha(int idt1, int idpk1, int idt2, int idpk2){
    cout << "### Batalha ###" << endl;
    _treinadores[idt1]->falar_nome();
    cout << " " << "(" << _treinadores[idt1]->usar_pokemon(idpk1)->getNomep() << ") vs. ";
    _treinadores[idt2]->falar_nome();
    cout << " " << "(" << _treinadores[idt2]->usar_pokemon(idpk2)->getNomep() << ")" << endl;
    _treinadores[idt1]->usar_pokemon(idpk1)->atacar(_treinadores[idt2]->usar_pokemon(idpk2));
    _treinadores[idt2]->usar_pokemon(idpk2)->imprimir_status();
    cout << "#########" << endl;
}