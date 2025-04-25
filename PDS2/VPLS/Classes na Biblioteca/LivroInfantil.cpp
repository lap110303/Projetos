#include "LivroInfantil.hpp"
#include <iostream>

LivroInfantil::LivroInfantil(std::string titulo, std::string autor, int anoPublicacao, int faixaEtariaMinima)
    : Livro(titulo, autor, anoPublicacao), _faixaEtariaMinima(faixaEtariaMinima) {
    _genero = "Infantil";
}

LivroInfantil::~LivroInfantil() {
    std::cout << "Destrutor LivroInfantil" << std::endl;
}

void LivroInfantil::print_info() {
    Livro::print_info();
    std::cout << "\t" << "Faixa etaria minima: " << _faixaEtariaMinima << std::endl;
}
