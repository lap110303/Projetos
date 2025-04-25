#include "CadastroUsuario.hpp"
#include <iostream>
#include <sstream>

int main() {
    CadastroUsuario cadastro;
    std::string linha;

    while (std::getline(std::cin, linha)) {
        std::istringstream iss(linha);
        char comando;
        iss >> comando;

        switch (comando) {
            case 'p': {
                std::string nome_playlist;
                iss >> nome_playlist;
                cadastro.adicionar_playlist(nome_playlist);
                break;
            }
            case 'f': {
                std::string nome_playlist, nome_filme, genero_filme;
                int duracao_filme;
                iss >> nome_playlist >> nome_filme >> genero_filme >> duracao_filme;
                cadastro.adicionar_filme(nome_playlist, nome_filme, genero_filme, duracao_filme);
                break;
            }
            case 'd': {
                std::string nome_playlist;
                int tipo_ordenacao;
                iss >> nome_playlist >> tipo_ordenacao;
                cadastro.imprimir_detalhes_playlist(nome_playlist, tipo_ordenacao);
                break;
            }
            case 'r': {
                std::string nome_playlist, nome_filme;
                iss >> nome_playlist >> nome_filme;
                cadastro.remover_filme_por_nome(nome_playlist, nome_filme);
                break;
            }
            case 'g': {
                cadastro.imprimir_informacoes_gerais();
                break;
            }
            default: {
                std::cerr << "Comando inválido!" << std::endl;
                break;
            }
        }
    }

    return 0;
}
