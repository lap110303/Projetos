#include "Cachorro.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

Cachorro::Cachorro(string nome, float peso, int tipo_pelo) 
    : Animal(nome, "Cachorro"), _peso(peso), _tipo_pelo(tipo_pelo) {}

Cachorro::~Cachorro() {
    cout << "Destrutor Cachorro" << endl;
}

void Cachorro::print_info() {
    Animal::print_info();
    cout << "\t" << "Peso: " << fixed << setprecision(2) << _peso << " kg" << endl;
    cout << "\t" << "Pelo: ";
    if(_tipo_pelo == 0){
        cout << "curto";
    }
    if(_tipo_pelo == 1){
        cout << "medio";
    }
    if(_tipo_pelo == 2){
        cout << "longo";
    }
    cout << endl;
}

float Cachorro::get_peso() {
    return _peso;
}

int Cachorro::get_tipo_pelo() {
    return _tipo_pelo;
}
