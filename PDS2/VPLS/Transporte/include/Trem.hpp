#ifndef TREM_HPP
#define TREM_HPP

#include "Transporte.hpp"
#include <vector>
#include <memory>

class Trem : public Transporte {
protected:
    std::vector<std::shared_ptr<Passageiro>> _passageiros;

public:
    Trem(int capacidade, float valor_passagem);
    virtual ~Trem();

    virtual bool pagar_passagem(std::shared_ptr<Passageiro> passageiro) override;
    virtual void mover() override;
    virtual void embarcar(std::shared_ptr<Passageiro> passageiro) override;
    virtual void desembarcar() override;
};

#endif