#include "Terreno.hpp"
#include "Comando.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) {
    // Verificar se os nomes dos arquivos foram passados como argumentos
    if (argc != 3) {
        std::cerr << "Uso: bin\\run.out <arquivo_mapa> <arquivo_comandos>" << std::endl;
        return 1;
    }

    // Nome dos arquivos (passados como argumentos)
    std::string arquivoMapa = argv[1];
    std::string arquivoComandos = argv[2];

    // Instanciar o terreno e carregar o mapa
    Terreno terreno;
    if (!terreno.carregarMapa(arquivoMapa)) {
        std::cerr << "Falha ao carregar o mapa: " << arquivoMapa << std::endl;
        return 1;
    }

    // Criar o gerenciador de comandos (Comando) com base no terreno
    Comando comandoGerenciador(&terreno);

    // Abrir o arquivo de comandos
    std::ifstream comandosFile(arquivoComandos);
    if (!comandosFile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de comandos: " << arquivoComandos << std::endl;
        return 1;
    }

    // Processar comandos linha a linha
    std::string linha;
    while (std::getline(comandosFile, linha)) {
        try {
            // Processa o comando no gerenciador
            comandoGerenciador.processarComando(linha);
        } catch (const std::exception& e) {
            std::cerr << "Erro ao processar comando: " << linha << "\n" << e.what() << std::endl;
        }
    }

    // Mensagem final com a quantidade de peças total
    comandoGerenciador.exibirTotalPecas();

    comandosFile.close();
    return 0;
}
