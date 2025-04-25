#include "Pokemon.hpp"
#include "PokemonEletrico.hpp"
#include "PokemonAquatico.hpp"
#include "PokemonExplosivo.hpp"
#include "Treinador.hpp"
#include "Campeonato.hpp"

#include <iostream>
#include <vector>
#include <string>

int main() {
    Campeonato campeonato;
    std::string letra;

    while (std::cin >> letra) {
        if (letra == "t") {
            std::string nome;
            std::cin >> nome;
            campeonato.cadastrar_treinador(nome);
        }
        if (letra == "i") {
            int idt;
            std::cin >> idt;
            campeonato.imprimir_informacoes_treinador(idt);
        }
        if (letra == "e") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, potencia_raio;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> potencia_raio;
            campeonato.cadastrar_pokemon_eletrico(idt, nome, tipo_ataque, forca_ataque, potencia_raio);
        }
        if (letra == "q") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, litros_jato;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> litros_jato;
            campeonato.cadastrar_pokemon_aquatico(idt, nome, tipo_ataque, forca_ataque, litros_jato);
        }
        if (letra == "x") {
            int idt;
            std::string nome, tipo_ataque;
            double forca_ataque, temperatura_explosao;
            std::cin >> idt >> nome >> tipo_ataque >> forca_ataque >> temperatura_explosao;
            campeonato.cadastrar_pokemon_explosivo(idt, nome, tipo_ataque, forca_ataque, temperatura_explosao);
        }
        if (letra == "h") {
            int idt1, idpk1, idt2, idpk2;
            std::cin >> idt1 >> idpk1 >> idt2 >> idpk2;
            campeonato.executar_batalha(idt1, idpk1, idt2, idpk2);
        }
    }

    return 0;
}