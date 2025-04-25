#ifndef POKEMON_H
#define POKEMON_H

#include <iostream>
#include <vector>
#include <string>

class Pokemon {
private:
    double _energia;

public:
    double _forca_ataque;
    std::string _nome;
    std::string _tipo_ataque;
    
    Pokemon(std::string nome, std::string tipo_ataque, double forca_ataque);
    void falar_nome();
    virtual void falar_tipo_ataque() = 0;
    void imprimir_status();
    void imprimir_informacoes();
    virtual double calcular_dano() = 0;
    void atacar(Pokemon* outro_pokemon);
    void receber_dano(double valor_dano);
};

#endif