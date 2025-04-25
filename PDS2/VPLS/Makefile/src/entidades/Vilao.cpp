#include "Vilao.hpp"

Vilao::Vilao(std::string _nome, std::string _universo, std::string _objetivo): Personagem(_nome, _universo), objetivo(_objetivo){}

Vilao::~Vilao(){}

void Vilao::introducao(){
    std::cout << "MWAHAHA sou " << nome << " da " << universo << " e meu objetivo eh " << objetivo << std::endl; 
}