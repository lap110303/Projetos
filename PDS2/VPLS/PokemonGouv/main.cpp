// NÃO ALTERE ESSA LINHA
#include <iostream>
#include "Campeonato.hpp" // Certifique-se de incluir seus cabeçalhos apropriados

int main() {
    Campeonato campeonato; // Crie uma instância da classe Campeonato
    std::string comando;
    
    while (std::cin >> comando) {
        if (comando == "t") {
            std::string nome;
            std::cin >> nome;
            campeonato.cadastrar_treinador(nome);
        } else if (comando == "i") {
            int idt;
            std::cin >> idt;
            campeonato.imprimir_informacoes_treinador(idt);
        } else if (comando == "e") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, potencia_raio;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> potencia_raio;
            campeonato.cadastrar_pokemon_eletrico(idt, nome, tipo_ataque, forca_ataque, potencia_raio);
        } else if (comando == "q") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, litros_jato;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> litros_jato;
            campeonato.cadastrar_pokemon_aquatico(idt, nome, tipo_ataque, forca_ataque, litros_jato);
        } else if (comando == "x") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, temperatura_explosao;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> temperatura_explosao;
            campeonato.cadastrar_pokemon_explosivo(idt, nome, tipo_ataque, forca_ataque, temperatura_explosao);
        } else if (comando == "h") {
            int idt1, idpk1, idt2, idpk2;
            std::cin >> idt1 >> idpk1 >> idt2 >> idpk2;
            campeonato.executar_batalha(idt1, idpk1, idt2, idpk2);
        } 
    }

    return 0;
}