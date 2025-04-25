#include "QuadTree.hpp"
#include <iostream>
#include <fstream>

int main(int argc, char *argv[]) {
    // Estratégia de Robustez
    if (argc < 3) {
        std::cerr << "Uso: " << argv[0] << " -b <arquivo_base> -e <arquivo_eventos>" << std::endl;
        return 1;
    }

    std::string arquivo_base, arquivo_eventos;

    // Processa os argumentos da linha de comando para obter os nomes dos arquivos
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-b") {
            arquivo_base = argv[++i];  // Arquivo de base
        } else if (std::string(argv[i]) == "-e") {
            arquivo_eventos = argv[++i];  // Arquivo de eventos
        }
    }

    // Estratégia de robustez
    if (arquivo_base.empty() || arquivo_eventos.empty()) {
        std::cerr << "Por favor, forneça os arquivos de base e eventos." << std::endl;
        return 1;
    }

    // Abre o arquivo de bases para leitura
    std::ifstream arquivo(arquivo_base);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de bases" << std::endl;
        return 1;
    }

    QuadTree qt;  // Cria uma instância da QuadTree
    std::string linha;

    // Lê cada linha do arquivo de bases e carrega as estações na QuadTree
    while (std::getline(arquivo, linha)) {
        Estacao* estacao = carregar_estacao(linha);  // Carrega a estação a partir da linha lida
        qt.inserir(estacao->x, estacao->y, estacao);  // Insere a estação na QuadTree
    }

    // Processa os eventos do arquivo de eventos, utilizando a QuadTree construída
    processar_eventos(arquivo_eventos, qt.raiz);

    // Destrói a QuadTree e libera a memória alocada
    destruir_quadtree(qt.raiz);

    return 0;
}
