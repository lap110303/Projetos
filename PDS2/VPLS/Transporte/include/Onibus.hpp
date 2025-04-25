#ifndef ONIBUS_HPP
#define ONIBUS_HPP

#include "Transporte.hpp"
#include <vector>
#include <memory>

class Onibus : public Transporte {
protected:
    std::vector<std::shared_ptr<Passageiro>> _passageiros;

public:
    Onibus(int capacidade, float valor_passagem);
    virtual ~Onibus();

    virtual bool pagar_passagem(std::shared_ptr<Passageiro> passageiro) override;
    virtual void mover() override;
    virtual void embarcar(std::shared_ptr<Passageiro> passageiro) override;
    virtual void desembarcar() override;
};

#endif