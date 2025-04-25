#ifndef LIVRO_H
#define LIVRO_H

#include <string>

class Livro {

private:
    int _id;
    static int _prox_id;
    std::string _titulo;
    std::string _autor;
    int _anoPublicacao;
    
public:
    Livro(std::string titulo, std::string autor, int anoPublicacao);
    virtual ~Livro();
    virtual void print_info();
    std::string _genero;

    friend class Biblioteca;
};

#endif
