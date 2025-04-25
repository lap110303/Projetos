#include "pokemon.hpp"

Pokemon::Pokemon(string nome, string tipo_ataque, double forca_ataque)
    : _nome(nome), _tipo_ataque(tipo_ataque), _forca_ataque(forca_ataque) {}

void Pokemon::falar_nome() {
    cout << _nome << "!" << endl;
}

void Pokemon::falar_tipo_ataque() {
    cout << _tipo_ataque << "!" << endl;
}

void Pokemon::imprimir_status() {
    cout << _nome << "!" << endl << "Energia: " << _energia << endl;
}

void Pokemon::imprimir_informacoes() {
    cout << "Pokemon: " << _nome << endl
         << "Tipo ataque: " << _tipo_ataque << endl
         << "Dano: " << calcular_dano() << endl
         << "Energia: " << _energia << endl; 
}

void Pokemon::atacar(Pokemon* outro_pokemon) {
    falar_nome();
    falar_tipo_ataque();
    double s = calcular_dano();
    cout << "Dano: " << s << endl;
    outro_pokemon->receber_dano(s);
}

void Pokemon::receber_dano(double valor_dano) {
    if ((_energia - valor_dano) < 0) {
        _energia = 0;
        cout << _nome << " morreu" << endl;
    } else {
        _energia -= valor_dano;
    }
}

double Pokemon::getForcaAtaque() {
    return _forca_ataque;
}

string Pokemon::getNomep() {
    return _nome;
}
