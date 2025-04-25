// NÃO ALTERE ESSA LINHA
#include "avaliacao_basica_controle.hpp"

#include <iostream>
#include <string>
#include "FilaProcessos.hpp"

int main() {
 FilaProcessos fila;
    char comando;
    
    while (std::cin >> comando) {
        if (comando == 'a') {
            std::string nome;
            int prioridade;
            std::cin >> nome >> prioridade;
            fila.adicionar_processo(nome, prioridade);
        } else if (comando == 'r') {
            Processo* removido = fila.remover_processo_maior_prioridade();
            if (removido != nullptr) delete removido;
        } else if (comando == 'i') {
            int id;
            std::cin >> id;
            Processo* removido = fila.remover_processo_por_id(id);
            if (removido != nullptr) delete removido;
        } else if (comando == 'p') {
            fila.imprimir_fila();
        } else if (comando == 'e') {
            int id;
            std::cin >> id;
            fila.estimativa_tempo_para_execucao(id);
        } else if (comando == 'b') {
            avaliacao_basica();
        }
    }
    
    return 0;
}