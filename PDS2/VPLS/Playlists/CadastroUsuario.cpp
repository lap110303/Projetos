#include "CadastroUsuario.hpp"

CadastroUsuario::~CadastroUsuario() {
    // Não acabou sendo necessário liberar memória
}


void CadastroUsuario::adicionar_playlist(std::string nome) {
    if (_playlists.find(nome) == _playlists.end()) {
        _playlists[nome] = Playlist(nome);
    }
}

void CadastroUsuario::adicionar_filme(std::string nome_playlist, std::string nome_filme, std::string genero_filme, int duracao_filme) {
    if (_playlists.find(nome_playlist) == _playlists.end()) {
        adicionar_playlist(nome_playlist);
    }
    _playlists[nome_playlist].adicionar_filme(nome_filme, genero_filme, duracao_filme);
}

void CadastroUsuario::remover_filme_por_nome(std::string nome_playlist, std::string nome_filme) {
    auto it = _playlists.find(nome_playlist);
    if (it != _playlists.end()) {
        it->second.remover_filme_por_nome(nome_filme);
    } else {
        std::cout << "Playlist nao encontrada: " << nome_playlist << "!" << std::endl;
    }
}

void CadastroUsuario::imprimir_detalhes_playlist(std::string nome_playlist, int tipo_ordenacao) {
    if (_playlists.empty()) {
        std::cout << "Nenhuma playlist cadastrada!" << std::endl;
        return;
    }

    auto it = _playlists.find(nome_playlist);
    if (it != _playlists.end()) {
        it->second.imprimir_playlist(tipo_ordenacao);
    } else {
        std::cout << "Playlist nao encontrada: " << nome_playlist << "!" << std::endl;
    }
}

void CadastroUsuario::imprimir_informacoes_gerais() {
    if (_playlists.empty()) {
        std::cout << "Nenhuma playlist cadastrada!" << std::endl;
        return;
    }

    for (auto it = _playlists.begin(); it != _playlists.end(); ++it) {
        std::cout << "Playlist: " << it->first << std::endl;
        it->second.imprimir_playlist(0);
    }
}
