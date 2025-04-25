#include "Ave.hpp"
#include <iostream>
using namespace std;

Ave::Ave(string nome, int tamanho) 
    : Animal(nome, "Ave"), _tamanho(tamanho) {}

Ave::~Ave() {
    cout << "Destrutor Ave" << endl;
}

void Ave::print_info() {
    Animal::print_info();
    cout <<  "\t" << "Tamanho: " << _tamanho << " cm" << endl;
}

int Ave::get_tamanho() {
    return _tamanho;
}
