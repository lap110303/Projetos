#include "Animal.hpp"

int Animal::_prox_id = 1;

Animal::Animal(string nome, string subtipo) 
    : _id(_prox_id++), _nome(nome), _subtipo(subtipo) {}

Animal::~Animal() {
    cout << "Destrutor Animal" << endl;
}

void Animal::print_info() {
    cout << "Info: Animal " << _id << endl;
    cout << "\t" << "Nome: " << _nome << endl;
    cout << "\t" << "Tipo: " << _subtipo << endl;
}

int Animal::get_id() {
    return _id;
}

string Animal::get_subtipo() {
    return _subtipo;
}

string Animal::get_nome() {
    return _nome;
}
