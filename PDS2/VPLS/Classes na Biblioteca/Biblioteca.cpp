#include "Biblioteca.hpp"
#include <iostream>

Biblioteca::Biblioteca() {}

void Biblioteca::add_livro(Livro* livro) {
    _livros.push_back(livro);
}

void Biblioteca::list_livros() {
    for (unsigned int i = 0; i < _livros.size(); i++) {
        _livros[i]->print_info();
    }
}

void Biblioteca::find_livro(int id) {
    for (unsigned int i = 0; i < _livros.size(); i++) {
        if (_livros[i]->_id == id) {
            _livros[i]->print_info();
            return;
        }
    }
    std::cout << "Erro ao pesquisar: livro com ID " << id << " nao encontrado" << std::endl;
}

void Biblioteca::remove_livro(int id) {
    for (auto it = _livros.begin(); it != _livros.end(); it++) {
        if ((*it)->_id == id) {
            delete *it;
            _livros.erase(it);
            return;
        }
    }
    std::cout << "Erro ao remover: livro com ID " << id << " nao encontrado" << std::endl;
}

void Biblioteca::destrutores(){
    for (unsigned int i = 0; i < _livros.size(); i++) {
        _livros[i]->~Livro();
    }
}
