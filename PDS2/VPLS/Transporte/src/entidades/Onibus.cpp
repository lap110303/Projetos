#include "Onibus.hpp"
#include <iostream>
#include <iomanip>
#include <cmath>

Onibus::Onibus(int capacidade, float valor_passagem) : Transporte(capacidade, valor_passagem) {}

Onibus::~Onibus() {
    std::cout << "Destrutor Onibus" << std::endl;
}

bool Onibus::pagar_passagem(std::shared_ptr<Passageiro> passageiro) {
    if (_passageiros.size() >= static_cast<size_t>(_capacidade)){
        return false;
    }
    if (passageiro->getIdade() >= 65) {
        std::cout << "Passagem gratuita:" << std::endl;
        std::cout << "\t" << "nome: " << passageiro->getNome() << std::endl;
        std::cout << "\t" << "idade: " << passageiro->getIdade() << std::endl;
        std::cout << "\t" << "saldo: R$ " << std::fixed << std::setprecision(2) << passageiro->getSaldoPassagem() << std::endl;
        return true;
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

void Onibus::mover() {
    double ocupacao_decimal = (static_cast<double>(_passageiros.size()) * 100.0) / static_cast<double>(_capacidade);
    int ocupacao = static_cast<int>(std::round(ocupacao_decimal));
    std::cout << "Onibus deslocando com " << ocupacao << "% da capacidade" << std::endl;
}

void Onibus::embarcar(std::shared_ptr<Passageiro> passageiro) {
    if (pagar_passagem(passageiro)) {
        _passageiros.push_back(passageiro);
    }
}

void Onibus::desembarcar() {
    std::cout << _passageiros.size() << " passageiros desembarcaram do onibus" << std::endl;
    _passageiros.clear();
}