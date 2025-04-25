#include <iostream>
#include "Biblioteca.hpp"
#include "LivroNaoFiccao.hpp"
#include "LivroInfantil.hpp"
#include "LivroFiccao.hpp"

int main() {
    Biblioteca biblioteca;
    std::string comando;

    while (std::cin >> comando) {

        if (comando == "list") {
            biblioteca.list_livros();
        }
        if (comando == "add_f") {
            std::string titulo, autor, subgenero;
            int anoPublicacao;
            std::cin >> titulo >> autor >> anoPublicacao >> subgenero;
            biblioteca.add_livro(new LivroFiccao(titulo, autor, anoPublicacao, subgenero));
        }
        if (comando == "add_nf") {
            std::string titulo, autor, assunto;
            int anoPublicacao;
            std::cin >> titulo >> autor >> anoPublicacao >> assunto;
            biblioteca.add_livro(new LivroNaoFiccao(titulo, autor, anoPublicacao, assunto));
        }
        if (comando == "add_i") {
            std::string titulo, autor;
            int anoPublicacao, faixaEtariaMinima;
            std::cin >> titulo >> autor >> anoPublicacao >> faixaEtariaMinima;
            biblioteca.add_livro(new LivroInfantil(titulo, autor, anoPublicacao, faixaEtariaMinima));
        }
        if (comando == "rm") {
            int id;
            std::cin >> id;
            biblioteca.remove_livro(id);
        }
        if (comando == "find") {
            int id;
            std::cin >> id;
            biblioteca.find_livro(id);
        }
    }
    biblioteca.destrutores();

    return 0;
}