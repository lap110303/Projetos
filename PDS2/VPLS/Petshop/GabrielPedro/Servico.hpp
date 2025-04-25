#ifndef SERVICO_HPP
#define SERVICO_HPP

#include "Orcamento.hpp"
#include <list>

class Servico {
private:
    list<Orcamento*> _orcamentos;

public:
    Servico();
    ~Servico();
    void calcular_orcamento(Animal* animal);
    void print_orcamentos();
    void remove_orcamento(int id_animal);
};

#endif
