#ifndef BIBLIOTECA_HPP
#define BIBLIOTECA_HPP

#include "Livro.hpp"
#include <vector>

class Biblioteca {
private:
    std::vector<Livro*> _livros;

public:
    Biblioteca();
    void add_livro(Livro* livro);
    void list_livros();
    void find_livro(int id);
    void remove_livro(int id);
    void destrutores();
};

#endif
