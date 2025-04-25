#ifndef ORCAMENTO_HPP
#define ORCAMENTO_HPP

#include "Animal.hpp"

class Orcamento {
private:
    Animal* _pet;
    float _valor;

public:
    Orcamento(Animal* pet, float valor);
    ~Orcamento();
    void print_info();
    int get_id_animal();
};

#endif
