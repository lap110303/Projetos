#ifndef GATO_HPP
#define GATO_HPP

#include "Animal.hpp"

class Gato : public Animal {
private:
    int _tipo_pelo;
    int _agressivo;

public:
    Gato(string nome, int tipo_pelo, int agressivo);
    ~Gato() override;
    void print_info() override;
    int get_tipo_pelo();
    int get_agressivo();
};

#endif
