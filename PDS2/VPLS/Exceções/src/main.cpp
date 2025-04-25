#include "Hotel.hpp"
#include "HotelExceptions.hpp"
#include "ReservaExceptions.hpp"
#include "QuartoExceptions.hpp"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>

void mover_quarto_para_inicio(std::vector<Quarto*>& quartos, Quarto* quarto) {
    auto it = std::find(quartos.begin(), quartos.end(), quarto);
    if (it != quartos.end()) {
        // Remove o quarto do vetor e insere na primeira posição
        quartos.erase(it);
        quartos.insert(quartos.begin(), quarto);
    }
}

int main() {
    Hotel hotel;
    std::string command;
    std::vector<Quarto*> ordem_reservas; // Vetor para manter a ordem das operações de reserva realizadas

    while (std::cin >> command) {
        try {
            if (command == "a") {
                int n_quarto;
                struct std::tm entrada = {}, saida = {};
                std::cin >> n_quarto >> std::get_time(&entrada, "%d/%m/%Y") >> std::get_time(&saida, "%d/%m/%Y");

                // Adiciona a reserva no quarto
                hotel.adiciona_reserva(n_quarto, entrada, saida);
                
                // Obtém o quarto recém-modificado
                Quarto* quarto_atual = nullptr;
                for (auto quarto : *hotel.get_quartos()) {
                    if (quarto->get_n_quarto() == n_quarto) {
                        quarto_atual = quarto;
                        break;
                    }
                }

                if (quarto_atual != nullptr) {
                    // Move o quarto para o início da lista de impressão
                    mover_quarto_para_inicio(*hotel.get_quartos(), quarto_atual);

                    // Verifica se o quarto já foi adicionado ao vetor de ordem de reservas
                    if (std::find(ordem_reservas.begin(), ordem_reservas.end(), quarto_atual) == ordem_reservas.end()) {
                        // Adiciona o quarto ao vetor de ordem de reservas se não estiver presente
                        ordem_reservas.push_back(quarto_atual);
                    }
                } else {
                    std::cerr << "Erro: Quarto não encontrado!" << std::endl;
                }
            } else if (command == "r") {
                int n_quarto;
                struct std::tm entrada = {};
                std::cin >> n_quarto >> std::get_time(&entrada, "%d/%m/%Y");

                // Remove a reserva do quarto
                hotel.remove_reserva(n_quarto, entrada);
                
                // Atualiza a ordem dos quartos após a remoção
                for (auto quarto : *hotel.get_quartos()) {
                    if (quarto->get_n_quarto() == n_quarto) {
                        mover_quarto_para_inicio(*hotel.get_quartos(), quarto);
                        break;
                    }
                }
            } else if (command == "p") {
                // Imprime os quartos na ordem das operações de reserva
                for (auto quarto : ordem_reservas) {
                    if (!quarto->get_reservas()->empty()) {
                        quarto->print_info();
                    }
                }
            }
        } catch (const std::exception& e) {
            std::cerr << e.what() << '\n';
        }
    }
    return 0;
}
