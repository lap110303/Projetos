#ifndef LIVROINFANTIL_HPP
#define LIVROINFANTIL_HPP

#include "Livro.hpp"
#include<string>

class LivroInfantil : public Livro {
private:
    int _faixaEtariaMinima;

public:
    LivroInfantil(std::string titulo, std::string autor, int anoPublicacao, int faixaEtariaMinima);
    virtual ~LivroInfantil();
    virtual void print_info() override;
};

#endif
