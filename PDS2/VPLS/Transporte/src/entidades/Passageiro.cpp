#include "Passageiro.hpp"

Passageiro::Passageiro(std::string nome, int idade, float saldo_passagem)
    : _nome(nome), _idade(idade), _saldo_passagem(saldo_passagem) {}

Passageiro::~Passageiro() {}

std::string Passageiro::getNome() const {
    return _nome;
}

int Passageiro::getIdade() const {
    return _idade;
}

float Passageiro::getSaldoPassagem() const {
    return _saldo_passagem;
}

void Passageiro::descontar_valor_passagem(float valor_passagem) {
    _saldo_passagem -= valor_passagem;
}