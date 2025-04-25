#include "LivroFiccao.hpp"
#include <iostream>

LivroFiccao::LivroFiccao(std::string titulo, std::string autor, int anoPublicacao, std::string subgenero)
    : Livro(titulo, autor, anoPublicacao), _subgenero(subgenero) {
    _genero = "Ficcao";
}

LivroFiccao::~LivroFiccao() {
    std::cout << "Destrutor LivroFiccao" << std::endl;
}

void LivroFiccao::print_info() {
    Livro::print_info();
    std::cout << "\t" <<"Subgenero: " << _subgenero << std::endl;
}
