#ifndef PACIENTE_H
#define PACIENTE_H

#include <string>

struct Paciente {

    Paciente(int prioridade = 0, std::string nome = "", int idade = 0, int senha = 1);
    int _senha;
    int _prioridade;
    int _idade;
    std::string _nome;
    int getID();
    int getPrioridade();
    int getIdade();
    std::string getNome();
    int tempo_reservado_atendimento();
    void imprimir_dados();

};

#endif