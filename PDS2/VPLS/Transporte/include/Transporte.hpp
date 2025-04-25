// Transporte.hpp
#ifndef TRANSPORTE_HPP
#define TRANSPORTE_HPP

#include "Passageiro.hpp"
#include <memory>

class Transporte {
protected:
    int _capacidade;
    float _valor_passagem;

public:
    Transporte(int capacidade, float valor_passagem);
    virtual ~Transporte();

    virtual bool pagar_passagem(std::shared_ptr<Passageiro> passageiro) = 0;
    virtual void mover() = 0;
    virtual void embarcar(std::shared_ptr<Passageiro> passageiro) = 0;
    virtual void desembarcar() = 0;
};

#endif // TRANSPORTE_HPP