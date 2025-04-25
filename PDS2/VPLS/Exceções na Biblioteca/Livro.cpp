#include "Livro.hpp"

bool Livro::estaDisponivel() {
    return _disponivel;
}

void Livro::emprestar(std::string usuario) {
    _disponivel = false;
    _emprestado_para = usuario;
}

void Livro::devolver() {
    _disponivel = true;
    _emprestado_para = "";
}
