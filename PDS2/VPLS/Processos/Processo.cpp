#include "Processo.hpp"

#include <iostream>

Processo::Processo(int id, std::string nome, int _prioridade){
    this->_prioridade = _prioridade;
    this->_nome = nome;
    this->_id = id;
}

int Processo::getID() {
    return this->_id;
}

int Processo::getPrioridade() {
    return this->_prioridade;
}

std::string Processo::getNome () {
    return this->_nome;
}

double Processo::tempo_reservado_processo() {
    if (_prioridade == 0){
        return 0.5;
    }
    
    if (_prioridade == 1){
        return 1.5;
    }
    
    if (_prioridade == 2){
        return 3.0;
    }
}

void Processo::imprimir_dados() {
    std::string _prioridade_codigo;
    if (_prioridade == 0){
        _prioridade_codigo = "BAIXA";
    }
    
    if (_prioridade == 1){
        _prioridade_codigo = "MEDIA";
    }
    
    if (_prioridade == 2){
        _prioridade_codigo = "ALTA";
    }
    
    std::cout  << _id << "\t" << _nome << "\t" << _prioridade_codigo << std::endl;
}