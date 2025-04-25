#include "Heroi.hpp"

Heroi::Heroi(std::string _nome, std::string _universo): Personagem(_nome, _universo) {}

Heroi::~Heroi(){}

void Heroi::setInimigo(Vilao *v){
    this->inimigo = v;
}

Vilao Heroi::getInimigo(){
    return *(this->inimigo);
}

void Heroi::introducao(){
    std::cout << "Ola, sou " << this->nome << " da " << this->universo << " e meu inimigo eh " << this->getInimigo().getNome() << std::endl;
}