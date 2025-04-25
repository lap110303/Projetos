#ifndef ROBO_HPP
#define ROBO_HPP

#include <string>
#include "Terreno.hpp"

class Robo {
private:
    int id; // Identiicador do Robo
    int posX, posY; // Posições do Robo
    int tamanhoFila, capacidadeFila; // Capacidade da fila de comandos do Robo
    int tamanhoHistorico, capacidadeHistorico; // Operações realizadas armazenadas
    int pecasColetadas; // Peças que estão com o robo
    bool emMissao; // Verifica se o robo está em missão
    std::string* filaComandos; // Fila de comandos
    std::string* historico; // Histórico
    Terreno* terreno; // Referência ao terreno
    static int totalPecasColetadas; // Total de peças coletadas por todos os robos

    void redimensionarFila(); // Modificar a fila do robo
    void redimensionarHistorico(); // Modificar o histórico do robo
    void registrarHistorico(const std::string& acao); // Registro do histórico do robo

public:
    Robo(int id, Terreno* terreno); // Construtor
    ~Robo(); // Destrutor

    void ativar(); // Operação de ATIVAR
    void adicionarComando(const std::string& comando, bool prioridade); // Adiciona um comando na fila
    void executarComandos(); // Operação de EXECUTAR, realizando as operações da fila
    void retornar(); // Operação de RETORNAR, trazendo o robo para a BASE
    void relatorio() const; // Operação de RELATORIO, printa as operações executadas
    void mover(int x, int y); // Operação de MOVER, move o robo
    void coletar(); // Operação de COLETAR, coleta a peça no local
    bool estaEmMissao() const; // Verificação se o robo está em missão
    void adicionarComandoPrioritario(const std::string& comando); // Ooperações com *, que entram primeiro na fila

    int getId() const; // Retorna ID
    int getPecasColetadas() const; // Retorna peças coletadas
    static int obterTotalPecasColetadas(); // Retorna total de peças
};

#endif