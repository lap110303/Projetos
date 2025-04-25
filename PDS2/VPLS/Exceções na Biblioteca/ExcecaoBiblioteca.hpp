#ifndef EXCECAOBIBLIOTECA_H
#define EXCECAOBIBLIOTECA_H

#include <exception>
#include <string>

namespace biblioteca_excp {
    class ExcecaoBiblioteca : public std::exception {
    private:
        std::string _message;

    public:
        ExcecaoBiblioteca(std::string message) : _message(message) {}
        const char* what() const throw() {
            return _message.c_str();
        }
    };

    class ExcecaoLivroIndisponivel : public ExcecaoBiblioteca {
    public:
        ExcecaoLivroIndisponivel(std::string titulo)
            : ExcecaoBiblioteca("Livro indisponível: " + titulo) {}
    };

    class ExcecaoDevolucaoNaoAutorizada : public ExcecaoBiblioteca {
    public:
        ExcecaoDevolucaoNaoAutorizada(std::string titulo, std::string usuario)
            : ExcecaoBiblioteca("Devolução não autorizada para o livro: " + titulo + " pelo usuário: " + usuario) {}
    };
}

#endif
