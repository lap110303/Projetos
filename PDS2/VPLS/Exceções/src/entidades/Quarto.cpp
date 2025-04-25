#include "Quarto.hpp"
#include "QuartoExceptions.hpp"
#include "ReservaExceptions.hpp"
#include "HotelExceptions.hpp"
#include <iostream>
#include <ctime>
#include <algorithm>

// Construtor
Quarto::Quarto(int n_quarto) : _n_quarto(n_quarto) {}

// Destrutor
Quarto::~Quarto() {
    for (auto reserva : _reservas) {
        delete reserva;
    }
    _reservas.clear();
}

// Obtém o número do quarto
int Quarto::get_n_quarto() {
    return _n_quarto;
}

// Obtém a lista de reservas
std::vector<Reserva*> *Quarto::get_reservas() {
    return &_reservas;
}

// Adiciona uma reserva ao quarto
void Quarto::adiciona_reserva(struct std::tm data_inicio, struct std::tm data_saida) {
    // Verifica se o quarto existe na coleção de quartos
    if (_n_quarto < 0 || _n_quarto >=6) {
        throw hotel_excp::quarto_nao_existe(_n_quarto);
    }

    // Verificar se a data de saída é anterior à data de entrada
    if (std::difftime(std::mktime(&data_saida), std::mktime(&data_inicio)) < 0) {
        throw reserva_excp::data_saida_antecede_entrada(data_inicio, data_saida);
    }

    // Verificar se a duração da reserva é menor que 1 dia
    if (std::difftime(std::mktime(&data_saida), std::mktime(&data_inicio)) == 0) {
        throw reserva_excp::periodo_muito_curto(data_inicio, data_saida);
    }

    Reserva nova_reserva(data_inicio, data_saida); // Cria um objeto reserva para comparação

    // Verificar se a reserva não sobrepõe nenhuma já existente
    for (auto reserva : _reservas) {
        if (reserva->comparar(nova_reserva) == 0) {
            throw quarto_excp::reserva_indisponivel(data_inicio, data_saida);
        }
    }

    // Adiciona a nova reserva
    _reservas.push_back(new Reserva(data_inicio, data_saida));

    // Ordena as reservas pelo método de comparação
    std::sort(_reservas.begin(), _reservas.end(), [](Reserva* a, Reserva* b) {
        return a->comparar(*b) < 0;  // Ordena pelo método de comparação, de forma ascendente
    });
}

void Quarto::remove_reserva(struct std::tm data_inicio) {
    // Verificar se o quarto não foi criado, ou seja, não tem reservas
    if (_reservas.empty()) {
        throw hotel_excp::quarto_nao_criado(_n_quarto);
    }

    // Percorrer as reservas e tentar encontrar a reserva a ser removida
    for (auto it = _reservas.begin(); it != _reservas.end(); ++it) {
        // Verificar se a reserva corresponde à data de início fornecida
        if (std::difftime(std::mktime((*it)->get_data_entrada()), std::mktime(&data_inicio)) == 0) {
            delete *it; // Libera a memória da reserva
            _reservas.erase(it); // Remove a reserva da lista
            return;
        }
    }

    // Se não encontrou a reserva, lançar a exceção de reserva não encontrada
    throw quarto_excp::reserva_nao_encontrada(_n_quarto, data_inicio);
}

// Imprime informações do quarto e suas reservas
void Quarto::print_info() {
    std::cout << "Quarto " << _n_quarto << " ---------" << std::endl;

    // Verifica se há reservas no quarto
    if (!_reservas.empty()) {
        // Imprime as reservas ordenadas
        for (auto reserva : _reservas) {
            reserva->print_info();
        }
    } else {
        std::cout << "Nenhuma reserva encontrada." << std::endl;
    }

    std::cout << "------------------" << std::endl;
}
