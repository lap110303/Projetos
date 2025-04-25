#include "Robo.hpp"
#include <iostream>

int Robo::totalPecasColetadas = 0;

Robo::Robo(int id, Terreno* terreno) // Construtor
    : id(id), posX(0), posY(0), tamanhoFila(0), capacidadeFila(100),
      tamanhoHistorico(0), capacidadeHistorico(100), pecasColetadas(0), emMissao(false), terreno(terreno) {
    filaComandos = new std::string[capacidadeFila];
    historico = new std::string[capacidadeHistorico];
}


Robo::~Robo() { // Destrutor
    delete[] filaComandos;
    delete[] historico;
}

void Robo::ativar() {
    if (!emMissao) {  // Só ativa se não estiver em missão
        std::cout << "BASE: ROBO " << id << " SAIU EM MISSAO" << std::endl;
        emMissao = true;  // Marca o robô como em missão
    } else { // Se o robo já estiver em missão
        std::cout << "BASE: ROBO " << id << " JA ESTA EM MISSAO" << std::endl;
    }
}

void Robo::adicionarComando(const std::string& comando, bool prioridade) { // Coloca uma operação na fila
    if (tamanhoFila == capacidadeFila) { // Caso a capacidade tenha se esgotado
        redimensionarFila();
    }
    if (prioridade) { // Operações com prioridade
        for (int i = tamanhoFila; i > 0; --i) {
            filaComandos[i] = filaComandos[i - 1];
        }
        filaComandos[0] = comando;
    } else { // Operações sem prioridade
        filaComandos[tamanhoFila] = comando;
    }
    ++tamanhoFila;
}

void Robo::executarComandos() { // Executa os comandos acumulados na fila
    if (!emMissao) {
        std::cout << "BASE: ROBO " << id << " NAO ESTA EM MISSAO" << std::endl;
        return;
    }

    for (int i = 0; i < tamanhoFila; ++i) {
        const std::string& comando = filaComandos[i];
        if (comando.find("MOVER") == 0) {
            int x, y;
            sscanf(comando.c_str(), "MOVER (%d,%d)", &x, &y);
            mover(x, y);  // Executa o comando MOVER
        } else if (comando == "COLETAR") {
            coletar();  // Executa o comando COLETAR
        }
    }

    tamanhoFila = 0; // Limpa a fila após executar os comandos
}

void Robo::retornar() { // Retorna o robo para a base
    if (!emMissao) { // Caso não esteja em missão
        std::cout << "BASE: ROBO " << id << " NAO ESTA EM MISSAO" << std::endl;
        return;
    }
    // Printa a mensagem e reinicia os atributos
    std::cout << "BASE: ROBO " << id << " RETORNOU PEÇAS " << pecasColetadas << std::endl;
    totalPecasColetadas += pecasColetadas;
    emMissao = false;
    pecasColetadas = 0;
    tamanhoFila = 0;
    tamanhoHistorico = 0;
}

void Robo::relatorio() const { // Printa o relatório
    for (int i = 0; i < tamanhoHistorico; ++i) {
        std::cout << historico[i] << std::endl;
    }
}

void Robo::mover(int x, int y) {
    // Checa se a posição está dentro do mapa
    if (x < 0 || y < 0 || x >= terreno->obterLinhas() || y >= terreno->obterColunas()) {
        registrarHistorico("ROBO " + std::to_string(id) + ": IMPOSSIVEL MOVER PARA (" + std::to_string(x) + "," + std::to_string(y) + ")");
        return;
    }

    // Verifica se a posição no terreno é válida
    char posicao = terreno->obterPosicao(x, y);
    if (posicao == 'O') {
        registrarHistorico("ROBO " + std::to_string(id) + ": IMPOSSIVEL MOVER PARA (" + std::to_string(x) + "," + std::to_string(y) + ")");
        return;
    }

    // Atualiza a posição do robô
    posX = x;
    posY = y;
    registrarHistorico("ROBO " + std::to_string(id) + ": MOVEU PARA (" + std::to_string(x) + "," + std::to_string(y) + ")");
}

void Robo::coletar() {
    // Verifica se a posição atual contém uma peça 'P'
    if (terreno->obterPosicao(posX, posY) == 'P') {
        registrarHistorico("ROBO " + std::to_string(id) + ": PEÇA COLETADA EM (" + std::to_string(posX) + "," + std::to_string(posY) + ")");
        ++pecasColetadas;
        terreno->atualizarPosicao(posX, posY, '.'); // Atualiza o terreno para remover a peça
    } else { // Caso não haja peças no local
        registrarHistorico("ROBO " + std::to_string(id) + ": IMPOSSIVEL COLETAR PEÇA EM (" + std::to_string(posX) + "," + std::to_string(posY) + ")");
    }
}

void Robo::redimensionarFila() { // Atualiza o tamanho da fila e a limpa
    capacidadeFila *= 2;
    std::string* novaFila = new std::string[capacidadeFila];
    for (int i = 0; i < tamanhoFila; ++i) {
        novaFila[i] = filaComandos[i];
    }
    delete[] filaComandos;
    filaComandos = novaFila;
}

void Robo::redimensionarHistorico() { // Atualiza o histórico e o limpa
    capacidadeHistorico *= 2;
    std::string* novoHistorico = new std::string[capacidadeHistorico];
    for (int i = 0; i < tamanhoHistorico; ++i) {
        novoHistorico[i] = historico[i];
    }
    delete[] historico;
    historico = novoHistorico;
}

void Robo::registrarHistorico(const std::string& acao) { // Registra o histórico
    if (tamanhoHistorico == capacidadeHistorico) {
        redimensionarHistorico();
    }
    historico[tamanhoHistorico++] = acao;
}

bool Robo::estaEmMissao() const { // Verifica se está em missão
    return emMissao;
}

void Robo::adicionarComandoPrioritario(const std::string& comando) { // Comandos com prioridade entram antes na fila
    if (tamanhoFila == capacidadeFila) {
        redimensionarFila();
    }

    // Desloca todos os comandos uma posição para trás
    for (int i = tamanhoFila; i > 0; --i) {
        filaComandos[i] = filaComandos[i - 1];
    }

    // Insere o novo comando no início da fila
    filaComandos[0] = comando;
    ++tamanhoFila;
}

int Robo::getId() const { // ID
    return id;
}

int Robo::getPecasColetadas() const { // Peças com cada robo
    return pecasColetadas;
}

int Robo::obterTotalPecasColetadas() { // Total de peças
    return totalPecasColetadas;
}