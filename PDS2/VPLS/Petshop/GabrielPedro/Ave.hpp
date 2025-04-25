#ifndef AVE_HPP
#define AVE_HPP

#include "Animal.hpp"

class Ave : public Animal {
private:
    int _tamanho;

public:
    Ave(string nome, int tamanho);
    ~Ave() override;
    void print_info() override;
    int get_tamanho();
};

#endif
