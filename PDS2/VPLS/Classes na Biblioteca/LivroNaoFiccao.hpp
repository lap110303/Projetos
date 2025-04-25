#ifndef LIVRONAOFICCAO_H
#define LIVRONAOFICCAO_H

#include "Livro.hpp"
#include <string>

class LivroNaoFiccao : public Livro {

private:
    std::string _assunto;

public:
    LivroNaoFiccao(std::string titulo, std::string autor, int anoPublicacao, std::string assunto);
    virtual ~LivroNaoFiccao();
    virtual void print_info() override;
};

#endif
