#include "Gato.hpp"
#include <iostream>
using namespace std;

Gato::Gato(string nome, int tipo_pelo, int agressivo) 
    : Animal(nome, "Gato"), _tipo_pelo(tipo_pelo), _agressivo(agressivo) {}

Gato::~Gato() {
    cout << "Destrutor Gato" << endl;
}

void Gato::print_info() {
    Animal::print_info();
    cout << "\t" << "Pelo: ";
    if(_tipo_pelo == 0){
        cout << "nenhuma";
    }
    if(_tipo_pelo == 1){
        cout << "curto";
    }
    if(_tipo_pelo == 2){
        cout << "medio";
    }
    if(_tipo_pelo == 3){
        cout << "longo";
    }
    cout << endl;

    cout << "\t" << "Agressivo: ";
    if(_agressivo == 0){
        cout << "baixo";
    }
    if(_agressivo == 1){
        cout << "moderado";
    }
    if(_agressivo == 2){
        cout << "alto";
    }
    cout << endl;
}

int Gato::get_tipo_pelo() {
    return _tipo_pelo;
}

int Gato::get_agressivo() {
    return _agressivo;
}
