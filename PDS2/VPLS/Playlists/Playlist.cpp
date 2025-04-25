#include "Playlist.hpp"

Playlist::Playlist() : _nome("") {}
Playlist::Playlist(std::string nome) : _nome(nome) {}

Playlist::~Playlist() {
    //Não acabou sendo necessário liberar memória
}

void Playlist::adicionar_filme(std::string nome, std::string genero, int duracao) {
    for (size_t i = 0; i < _filmes.size(); i++) {
        if (_filmes[i].get_titulo() == nome) {
            std::cout << "Filme já existe na playlist: " << nome << std::endl;
            return;
        }
    }
    _filmes.push_back(Filme(nome, genero, duracao));
}

void Playlist::remover_filme_por_nome(std::string nome) {
    for (size_t i = 0; i < _filmes.size(); i++) {
        if (_filmes[i].get_titulo() == nome) {
            _filmes.erase(_filmes.begin() + i);
            return;
        }
    }
    std::cout << "Filme nao encontrado: " << nome << "!" << std::endl;
}

void Playlist::ordenar_por_nome() {
    for (size_t i = 0; i < _filmes.size(); i++) {
        for (size_t j = i + 1; j < _filmes.size(); j++) {
            if (_filmes[i].get_titulo() > _filmes[j].get_titulo()) {
                Filme temp = _filmes[i];
                _filmes[i] = _filmes[j];
                _filmes[j] = temp;
            }
        }
    }
}

void Playlist::ordenar_por_duracao() {
    for (size_t i = 0; i < _filmes.size(); i++) {
        for (size_t j = i + 1; j < _filmes.size(); j++) {
            if (_filmes[i].get_duracao() > _filmes[j].get_duracao()) {
                Filme temp = _filmes[i];
                _filmes[i] = _filmes[j];
                _filmes[j] = temp;
            }
        }
    }
}

void Playlist::imprimir_playlist(int tipo_ordenacao) {
    if (_filmes.empty()) {
        std::cout << "Nenhum filme cadastrado na playlist: " << _nome << "!" << std::endl;
        return;
    }

    if (tipo_ordenacao == 0) {
        ordenar_por_nome();
    } else if (tipo_ordenacao == 1) {
        ordenar_por_duracao();
    }

    int tempo_total = 0;
    for (size_t i = 0; i < _filmes.size(); i++) {
        _filmes[i].imprimir_dados();
        tempo_total += _filmes[i].get_duracao();
    }

    int horas = tempo_total / 60;
    int minutos = tempo_total % 60;
    std::cout << "Tempo total: " << horas << ":" << minutos << "h" << std::endl;
}
