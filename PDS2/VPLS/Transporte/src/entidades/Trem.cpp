#include "Trem.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

Trem::Trem(int capacidade, float valor_passagem) : Transporte(capacidade, valor_passagem) {}

Trem::~Trem() {
    std::cout << "Destrutor Trem" << std::endl;
}

bool Trem::pagar_passagem(std::shared_ptr<Passageiro> passageiro) {
    if (_passageiros.size() >= static_cast<size_t>(_capacidade)){
        return false;
    }
    if (passageiro->getSaldoPassagem() >= _valor_passagem) {
        passageiro->descontar_valor_passagem(_valor_passagem);
        return true;
    } else {
        std::cout << "Saldo insuficiente:" << std::endl;
        std::cout << "\t" << "nome: " << passageiro->getNome() << std::endl;
        std::cout << "\t" << "saldo: R$ " << std::fixed << std::setprecision(2) << passageiro->getSaldoPassagem() << std::endl;
        return false;
    }
}

void Trem::mover() {
    double ocupacao_decimal = (static_cast<double>(_passageiros.size()) * 100.0) / static_cast<double>(_capacidade);
    int ocupacao = static_cast<int>(std::round(ocupacao_decimal));
    std::cout << "Deslocando com " << ocupacao << "% da capacidade" << std::endl;
}

void Trem::embarcar(std::shared_ptr<Passageiro> passageiro) {
    if (pagar_passagem(passageiro)) {
        _passageiros.push_back(passageiro);
    }
}

void Trem::desembarcar() {
    std::cout << _passageiros.size() << " passageiros desembarcaram do trem" << std::endl;
    _passageiros.clear();
}