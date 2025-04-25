#ifndef PLAYLIST_HPP
#define PLAYLIST_HPP

#include "Filme.hpp"
#include <vector>
#include <string>
#include <iostream>

class Playlist {
private:
    std::string _nome;
    std::vector<Filme> _filmes;

    void ordenar_por_nome();
    void ordenar_por_duracao();

public:
    Playlist();
    Playlist(std::string nome);
    ~Playlist();

    void adicionar_filme(std::string nome, std::string genero, int duracao);
    void remover_filme_por_nome(std::string nome);
    void imprimir_playlist(int tipo_ordenacao);
};

#endif
