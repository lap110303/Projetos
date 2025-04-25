#ifndef COMANDO_HPP
#define COMANDO_HPP

#include "Robo.hpp"
#include "Terreno.hpp"
#include <string>

struct Nodo { // Lista encadeada para armazenar os robos
    Robo* robo;
    Nodo* proximo;
};

class Comando {
private:
    Nodo* listaRobos;
    Terreno* terreno;
    
public:
    Comando(Terreno* terreno); // Construtor
    ~Comando(); // Destrutor
    
    Robo* criarRobo(int id); // Cria um novo robo e coloca na lista encadeada
    Robo* encontrarRobo(int id); // Procura o robo na lista encadeada
    
    void processarComando(const std::string& comando); // Processa os comandos no .txt e verifica a operação
    void relatorioRobo(int id); // Realiza o relatório do robo
    void exibirTotalPecas(); // Exibe o total de peças coletadas
};

#endif
