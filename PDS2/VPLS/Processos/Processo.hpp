#ifndef PROCESSOS_H
#define PROCESSOS_H

#include <string>

struct Processo {

    Processo(int id,std::string nome = "", int prioridade = 0);
    int _id;
    int _prioridade;
    std::string _nome;
    int getID();
    int getPrioridade();
    std::string getNome();
    double tempo_reservado_processo();
    void imprimir_dados();

};

#endif