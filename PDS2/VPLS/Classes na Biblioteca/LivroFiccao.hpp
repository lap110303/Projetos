#ifndef LIVROFICCAO_H
#define LIVROFICCAO_H

#include "Livro.hpp"
#include <string>

class LivroFiccao : public Livro {
private:
    std::string _subgenero;

public:
    LivroFiccao(std::string titulo, std::string autor, int anoPublicacao, std::string subgenero);
    virtual ~LivroFiccao();
    virtual void print_info() override;
};

#endif
