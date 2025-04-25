#ifndef METRO_HPP
#define METRO_HPP

#include "Trem.hpp"

class Metro : public Trem {
public:
    Metro(int capacidade, float valor_passagem);
    virtual ~Metro();

    virtual bool pagar_passagem(std::shared_ptr<Passageiro> passageiro) override;
    virtual void desembarcar() override;
};

#endif