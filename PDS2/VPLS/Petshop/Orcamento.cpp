#include "Orcamento.hpp"
#include <iostream>
#include <iomanip>
using namespace std;

Orcamento::Orcamento(Animal* pet, float valor) 
    : _pet(pet), _valor(valor) {}

Orcamento::~Orcamento() {
    cout << "Destrutor Orcamento" << endl;
}

void Orcamento::print_info() {
    cout << "============================" << endl;
    cout << "Info: Orcamento" << endl;
    cout << "Valor: R$ " << fixed << setprecision(2) << _valor << endl;
    cout << "Pet ";
    _pet->print_info();
    cout << "============================" << endl;
}

int Orcamento::get_id_animal() {
    return _pet->get_id();
}
