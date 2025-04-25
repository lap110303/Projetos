#include "Livro.hpp"
#include <iostream>

Livro::Livro(std::string titulo, std::string autor, int anoPublicacao)
    : _id(_prox_id++), _titulo(titulo), _autor(autor), _anoPublicacao(anoPublicacao), _genero("Livro") {}

Livro::~Livro() {
    std::cout << "Destrutor Livro" << std::endl;
}

void Livro::print_info() {
    std::cout << "Info: Livro " << _id << std::endl;
    std::cout << "\t" << "Titulo: " << _titulo << std::endl;
    std::cout << "\t" << "Autor: " << _autor << std::endl;
    std::cout << "\t" << "Ano de publicacao: " << _anoPublicacao << std::endl;
    std::cout << "\t" << "Genero: " << _genero << std::endl;
}

int Livro::_prox_id = 1;
