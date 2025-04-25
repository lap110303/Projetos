#include "Personagem.hpp"

Personagem::Personagem(std::string _nome, std::string _universo): nome(_nome), universo(_universo) {}

Personagem::~Personagem(){}

std::string Personagem::getNome(){
    return nome;
}

void Personagem::setNome(std::string _nome){
    this->nome = _nome;
}