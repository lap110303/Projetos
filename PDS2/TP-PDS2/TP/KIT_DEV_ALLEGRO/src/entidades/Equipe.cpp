#include <iostream>
#include <vector>
#include "Pokemon.hpp"
#include "Equipe.hpp"

void Equipe::adicionar_pokemon(Pokemon* pokemon) {
    time.push_back(pokemon);
}

void Equipe::remover_pokemon(Pokemon* pokemon) {
    // Find and remove the specified Pokemon from the team
    for (auto it = time.begin(); it != time.end(); ++it) {
        if (*it == pokemon) {
            time.erase(it);
            break;
        }
    }
}

void Equipe::imprimir_time() {
    std::cout << "Time de Pokemon:" << std::endl;
    for (const auto& pokemon : time) {
        std::cout << "- " << pokemon->getNome() << std::endl;
    }
}

void Equipe::trocar_pokemon_ativo(Pokemon* pokemon) {
    _pokemon_ativo = pokemon->getNome();
}


