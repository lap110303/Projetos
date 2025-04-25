#ifndef CADASTROUSUARIO_HPP
#define CADASTROUSUARIO_HPP

#include "Playlist.hpp"
#include <map>
#include <string>
#include <iostream>

class CadastroUsuario {
private:
    std::map<std::string, Playlist> _playlists;

public:
    ~CadastroUsuario();
    
    void adicionar_playlist(std::string nome);
    void adicionar_filme(std::string nome_playlist, std::string nome_filme, std::string genero_filme, int duracao_filme);
    void remover_filme_por_nome(std::string nome_playlist, std::string nome_filme);
    void imprimir_detalhes_playlist(std::string nome_playlist, int tipo_ordenacao);
    void imprimir_informacoes_gerais();
};

#endif
