#include "Comando.hpp"
#include <iostream>
#include <sstream>

Comando::Comando(Terreno* terreno) // Construtor
    : listaRobos(nullptr), terreno(terreno) {}

Comando::~Comando() { // Destrutor
    Nodo* atual = listaRobos;
    while (atual) {
        Nodo* proximo = atual->proximo;
        delete atual->robo;  // Deleta o robô
        delete atual;  // Deleta o nó
        atual = proximo;
    }
}

Robo* Comando::criarRobo(int id) { // Criação de um novo robô
    Robo* robo = new Robo(id, terreno);

    // Cria um novo nó para armazenar o robô na lista encadeada
    Nodo* novoNodo = new Nodo;
    novoNodo->robo = robo;
    novoNodo->proximo = listaRobos;

    // Atualiza o início da lista
    listaRobos = novoNodo;

    return robo;
}

Robo* Comando::encontrarRobo(int id) { // Encontra um robô na lista
    Nodo* atual = listaRobos;
    while (atual) {
        if (atual->robo->getId() == id) {
            return atual->robo;
        }
        atual = atual->proximo;
    }
    return nullptr;
}

void Comando::processarComando(const std::string& comando) {
    std::string acao;
    int id;

    // Criação do stream para analisar o comando
    std::istringstream comandoStream(comando);
    if (!(comandoStream >> acao)) {
        std::cerr << "Comando inválido: " << comando << std::endl;
        return;
    }

    if (acao == "ATIVAR") { // Comando de ATIVAR
        if (!(comandoStream >> id)) {
            std::cerr << "Comando ATIVAR inválido: " << comando << std::endl;
            return;
        }

        Robo* robo = encontrarRobo(id);
        if (!robo) {
            robo = criarRobo(id); // Cria o robô se não existir
        }
        robo->ativar();

    } else if (acao == "MOVER" || acao == "*MOVER") { // Comando de MOVER
        bool prioridade = (acao[0] == '*');  // Verifica se é comando prioritário
        if (prioridade) acao.erase(0, 1);    // Remove o '*' do comando

        if (!(comandoStream >> id)) {
            std::cerr << "Comando MOVER inválido: " << comando << std::endl;
            return;
        }

        Robo* robo = encontrarRobo(id);
        if (robo) {
            char parenteseInicio, parenteseFim, virgula;
            int x, y;

            if (!(comandoStream >> parenteseInicio >> x >> virgula >> y >> parenteseFim) ||
                parenteseInicio != '(' || parenteseFim != ')' || virgula != ',') {
                std::cerr << "Comando MOVER inválido: " << comando << std::endl;
                return;
            }

            std::string comandoCompleto = "MOVER (" + std::to_string(x) + "," + std::to_string(y) + ")";
            robo->adicionarComando(comandoCompleto, prioridade);
        }

    } else if (acao == "COLETAR" || acao == "*COLETAR") { // Comando de COLETAR
        bool prioridade = (acao[0] == '*');  // Verifica se é comando prioritário
        if (prioridade) acao.erase(0, 1);    // Remove o '*' do comando

        if (!(comandoStream >> id)) {
            std::cerr << "Comando COLETAR inválido: " << comando << std::endl;
            return;
        }

        Robo* robo = encontrarRobo(id);
        if (robo) {
            std::string comandoCompleto = "COLETAR";
            robo->adicionarComando(comandoCompleto, prioridade);
        }

    } else if (acao == "EXECUTAR") { // Comando de EXECUTAR
        if (!(comandoStream >> id)) {
            std::cerr << "Comando EXECUTAR inválido: " << comando << std::endl;
            return;
        }

        Robo* robo = encontrarRobo(id);
        if (robo) {
            robo->executarComandos();
        } else {
            std::cout << "BASE: ROBO " << id << " NAO ESTA EM MISSAO" << std::endl;
        }

    } else if (acao == "RELATORIO") { // Comando de RELATORIO
        if (!(comandoStream >> id)) {
            std::cerr << "Comando RELATORIO inválido: " << comando << std::endl;
            return;
        }

        relatorioRobo(id);

    } else if (acao == "RETORNAR") { // Comando de RETORNAR
        if (!(comandoStream >> id)) {
            std::cerr << "Comando RETORNAR inválido: " << comando << std::endl;
            return;
        }

        Robo* robo = encontrarRobo(id);
        if (robo) {
            robo->retornar();
        }

    } else { // Comando desconhecido
        std::cerr << "Comando desconhecido: " << comando << std::endl;
    }
}

void Comando::relatorioRobo(int id) { // Exibe o relatório de um robo
    const Robo* robo = encontrarRobo(id);
    if (robo) {
        robo->relatorio();
    }
}

void Comando::exibirTotalPecas() { // Exibe o total de peças
    std::cout << "BASE: TOTAL DE PEÇAS " << Robo::obterTotalPecasColetadas() << std::endl;
}

