// NÃO ALTERE ESSA LINHA
#include "avaliacao_basica_atendimento.hpp"

#include <iostream>
#include "FilaAtendimento.hpp"

using namespace std;

int main() {
    
    FilaAtendimento fila;
    char letra;
    
    while (cin >> letra) {
        if (letra == 'a') {
            int _prioridade, _idade;
            string _nome;
            cin >> _prioridade >> _nome >> _idade;
            fila.adicionar_paciente(_prioridade, _nome, _idade);
        }
            
        if (letra == 'p') {
            fila.imprimir_fila();
        }
            
        if (letra == 'r') {
            fila.chamar_paciente_por_prioridade();
        }
            
        if (letra == 'i') {
            fila.chamar_paciente_por_idade();
        }
            
        if (letra == 'm') {
            Paciente* paciente;
            paciente = fila.buscar_paciente_mais_velho();
            paciente->imprimir_dados();
        }
            
        if (letra == 'e') {
            int senha;
            cin >> senha;
            fila.estimativa_tempo_para_atendimento(senha);
        }
            
        if (letra == 'b') { 
            avaliacao_basica();
        }
    }

    return 0;
}