#ifndef FILME_HPP
#define FILME_HPP

#include <string>
#include <iostream>

class Filme {
private:
    std::string _titulo;
    std::string _genero;
    int _duracao;

public:
    Filme(std::string titulo, std::string genero, int duracao);
    ~Filme();

    void imprimir_dados();

    //funções auxiliares
    std::string get_titulo();
    std::string get_genero();
    int get_duracao();
};

#endif
