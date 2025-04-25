#include "Filme.hpp"

Filme::Filme(std::string titulo, std::string genero, int duracao) : _titulo(titulo), _genero(genero), _duracao(duracao) {}

Filme::~Filme() {
    // Não acabou sendo necessário liberar memória
}

void Filme::imprimir_dados() {
    std::cout << _titulo << " (" << _genero << ")" << "\t" <<  _duracao << " minutos" << std::endl;
}

std::string Filme::get_titulo() {
    return _titulo;
}

std::string Filme::get_genero() {
    return _genero;
}

int Filme::get_duracao() {
    return _duracao;
}