#ifndef QUARTO_H
#define QUARTO_H

#include "QuartoExceptions.hpp"
#include "Reserva.hpp"

#include <vector>

class Quarto {
private:
    int _n_quarto;
    std::vector<Reserva*> _reservas;

public:
    Quarto(int n_quarto);
    ~Quarto();

    int get_n_quarto();
    std::vector<Reserva*> *get_reservas();
    void adiciona_reserva(struct std::tm data_inicio, struct std::tm data_saida);
    void remove_reserva(struct std::tm data_inicio);
    void print_info();
};

#endif