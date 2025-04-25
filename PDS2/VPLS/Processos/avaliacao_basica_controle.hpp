#ifndef AVALIACAO_H
#define AVALIACAO_H

#include <iostream>
#include "FilaProcessos.hpp"

using namespace std;

void avaliacao_basica() {
    
    FilaProcessos fila;

    cout << "----------" << endl;
    Processo *maior_prioridade = fila.remover_processo_maior_prioridade();
    if (maior_prioridade == nullptr) {
        cout << "CORRECT" << endl; 
    } else {
        cout << "ERROR"; 
    }
    cout << "----------" << endl;

    fila.adicionar_processo("Proc1", 0);
    fila.adicionar_processo("Proc2", 1);
    fila.adicionar_processo("Proc3", 2);
    fila.adicionar_processo("Proc4", 0);
    fila.adicionar_processo("Proc5", 1);
    fila.adicionar_processo("Proc6", 2);

    fila.imprimir_fila();
    cout << "----------" << endl;

    maior_prioridade = fila.remover_processo_maior_prioridade();
    maior_prioridade->imprimir_dados();
    delete maior_prioridade;
    cout << "----------" << endl;

    fila.imprimir_fila();
    cout << "----------" << endl;
    
    Processo *processo_id = fila.remover_processo_por_id(1);
    processo_id->imprimir_dados();
    delete processo_id;
    cout << "----------" << endl;

    processo_id = fila.remover_processo_por_id(1);
    if (processo_id == nullptr) {
        cout << "CORRECT" << endl; 
    } else {
        cout << "ERROR"; 
    }
    cout << "----------" << endl;

    fila.imprimir_fila();
    cout << "----------" << endl;
}

#endif