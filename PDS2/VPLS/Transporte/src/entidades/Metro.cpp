#include "Metro.hpp"
#include <iostream>
#include <iomanip>

Metro::Metro(int capacidade, float valor_passagem) : Trem(capacidade, valor_passagem) {}

Metro::~Metro() {
    std::cout << "Destrutor Metro" << std::endl;
}

bool Metro::pagar_passagem(std::shared_ptr<Passageiro> passageiro) {
    float valor_total = _valor_passagem + 0.40f;
    if (_passageiros.size() >= static_cast<size_t>(_capacidade)){
        return false;
    }
    if (passageiro->getSaldoPassagem() >= valor_total) {
        passageiro->descontar_valor_passagem(valor_total);
        return true;
    } else {
        std::cout << "Saldo insuficiente:" << std::endl;
        std::cout << "\t" << "nome: " << passageiro->getNome() << std::endl;
        std::cout << "\t" << "saldo: R$ " << std::fixed << std::setprecision(2) << passageiro->getSaldoPassagem() << std::endl;
        return false;
    }
}

void Metro::desembarcar() {
    std::cout << _passageiros.size() << " passageiros desembarcaram do metro" << std::endl;
    _passageiros.clear();
}