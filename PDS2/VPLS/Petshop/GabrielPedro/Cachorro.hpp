#ifndef CACHORRO_HPP
#define CACHORRO_HPP

#include "Animal.hpp"

class Cachorro : public Animal {
private:
    float _peso;
    int _tipo_pelo;

public:
    Cachorro(string nome, float peso, int tipo_pelo);
    ~Cachorro() override;
    void print_info() override;
    float get_peso();
    int get_tipo_pelo();
};

#endif
