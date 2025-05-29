#include "Treinador.hpp"


Treinador::Treinador(std::string nome, std::string regiao): _nome(nome), _regiao(regiao){}

Treinador::~Treinador(){}

std::string Treinador::getNome(){
    return _nome;
}

std::string Treinador::getRegiao(){
    return _regiao;
}

void Treinador::abrirMochila(){
    _status_mochila = "aberta";
}

void Treinador::fecharMochila(){
    _status_mochila = "fechada";
}

std::string Treinador::imprimirInfos(){
    return "Nome: " + _nome + "\nRegião: " + _regiao;
}


