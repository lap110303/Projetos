// Hotel.cpp
#include "Hotel.hpp"
#include <iostream>
#include <stdexcept>
#include <algorithm>

Hotel::Hotel() {
    for (int i = 0; i <= NUMERO_MAX_QUARTO; i++) {
        _quartos.push_back(new Quarto(i));
    }
}

Hotel::~Hotel() {
    for (auto quarto : _quartos) {
        delete quarto;
    }
}

std::vector<Quarto*>* Hotel::get_quartos() {
    return &_quartos;
}

Quarto* Hotel::get_quarto_pelo_n(int n_quarto) {
    for (auto quarto : _quartos) {
        if (quarto->get_n_quarto() == n_quarto) {
            return quarto;
        }
    }
    throw std::runtime_error("Quarto não encontrado."); // Altere conforme necessário
}

void Hotel::adiciona_reserva(int n_quarto, struct std::tm data_entrada, struct std::tm data_saida) {
    Quarto* quarto = get_quarto_pelo_n(n_quarto); // Obter o quarto pelo número
    quarto->adiciona_reserva(data_entrada, data_saida); // Adicionar reserva ao quarto
}


void Hotel::remove_reserva(int n_quarto, struct std::tm data_entrada) {
    Quarto* quarto = get_quarto_pelo_n(n_quarto);
    quarto->remove_reserva(data_entrada);
}

void Hotel::print_info() {
    // Ordena os quartos pela ordem de inserção das operações realizadas
    std::vector<Quarto*> quartos_ordenados = _quartos;  // Cria uma cópia do vetor de quartos
    std::sort(quartos_ordenados.begin(), quartos_ordenados.end(), [](Quarto* a, Quarto* b) {
        return a->get_n_quarto() < b->get_n_quarto();  // Ordena por número de quarto, ou qualquer critério desejado
    });

    for (auto quarto : quartos_ordenados) {
        if (!quarto->get_reservas()->empty()) {
            quarto->print_info();  // Imprime as informações do quarto
        }
    }
}
