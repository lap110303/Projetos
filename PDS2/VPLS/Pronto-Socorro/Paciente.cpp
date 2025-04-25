#include "Paciente.hpp"

#include <iostream>

Paciente::Paciente(int prioridade, std::string nome, int idade, int senha){
    this->_prioridade = prioridade;
    this->_nome = nome;
    this->_idade = idade;
    this->_senha = senha;
}

int Paciente::getID() {
    return this->_senha;
}

int Paciente::getPrioridade() {
    return this->_prioridade;
}

int Paciente::getIdade() {
    return this->_idade;
}

std::string Paciente::getNome () {
    return this->_nome;
}

int Paciente::tempo_reservado_atendimento() {
    if (_prioridade == 0){
        return 20;
    }
    
    if (_prioridade == 1){
        return 18;
    }
    
    if (_prioridade == 2){
        return 12;
    }
    
    if (_prioridade == 3){
        return 8;
    }
    
    if (_prioridade == 4){
        return 6;
    }
    else
        return 0;
}

void Paciente::imprimir_dados() {
    int prior = this->_prioridade;
    std::string prioridade_codigo;
    if (prior == 0){
        prioridade_codigo = "V";
    }
    
    if (prior == 1){
        prioridade_codigo = "L";
    }
    
    if (prior == 2){
        prioridade_codigo = "A";
    }
    
    if (prior == 3){
        prioridade_codigo = "D";
    }
    
    if (prior == 4){
        prioridade_codigo = "Z";
    }
    std::cout  << prioridade_codigo << "\t" << _senha << "\t" << _nome << "\t" << _idade << std::endl;
}