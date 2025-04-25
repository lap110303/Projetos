#include "Biblioteca.hpp"
#include "Livro.hpp"
#include "ExcecaoBiblioteca.hpp"

void Biblioteca::adicionarLivro(std::string titulo) {
    livros[titulo] = new Livro(titulo);
}

void Biblioteca::emprestarLivro(std::string titulo, std::string usuario) {
    if (livros.find(titulo) != livros.end()) {
        Livro *livro = livros[titulo];
        if (livro->estaDisponivel()) {
            livro->emprestar(usuario);
        } else {
            throw biblioteca_excp::ExcecaoLivroIndisponivel(titulo);
        }
    } else {
        throw biblioteca_excp::ExcecaoLivroIndisponivel(titulo);
    }
}

void Biblioteca::devolverLivro(std::string titulo, std::string usuario) {
    if (livros.find(titulo) != livros.end()) {
        Livro *livro = livros[titulo];
        if (!livro->estaDisponivel() && livro->getEmprestadoPara() == usuario) {
            livro->devolver();
        } else {
            throw biblioteca_excp::ExcecaoDevolucaoNaoAutorizada(titulo, usuario);
        }
    } else {
        throw biblioteca_excp::ExcecaoDevolucaoNaoAutorizada(titulo, usuario);
    }
}

void Biblioteca::listarLivros() {
    std::cout << "Livros na Biblioteca:" << std::endl;
    for (const auto &livro : livros) {
        std::cout << livro.first << " - ";
    
        if (livro.second->estaDisponivel()) {
            std::cout << "Disponível";
        } else {
            std::cout << "Indisponível";
        }
    
        std::cout << std::endl;
    }

}
