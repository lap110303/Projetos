#ifndef POKEMON_HPP
#define POKEMON_HPP

#include <iostream>
#include <string>

using namespace std;

class Pokemon {
    string _nome;
    string _tipo_ataque;
    double _forca_ataque; 
    double _energia = 100;

public:
    Pokemon(string nome, string tipo_ataque, double forca_ataque);
    void falar_nome();
    virtual void falar_tipo_ataque();
    void imprimir_status();
    void imprimir_informacoes();
    virtual double calcular_dano() = 0;
    void atacar(Pokemon* outro_pokemon);
    void receber_dano(double valor_dano);
    double getForcaAtaque();
    string getNomep();
};

#endif
