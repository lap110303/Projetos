#ifndef PASSAGEIRO_HPP
#define PASSAGEIRO_HPP

#include <string>

class Passageiro {
private:
    std::string _nome;
    int _idade;
    float _saldo_passagem;

public:
    Passageiro(std::string nome, int idade, float saldo_passagem);
    ~Passageiro();

    std::string getNome() const;
    int getIdade() const;
    float getSaldoPassagem() const;

    void descontar_valor_passagem(float valor_passagem);
};

#endif