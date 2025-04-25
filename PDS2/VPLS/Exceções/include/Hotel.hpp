#ifndef HOTEL_H
#define HOTEL_H

#define NUMERO_MAX_QUARTO 6

#include "HotelExceptions.hpp"
#include "Quarto.hpp"

#include <vector>

class Hotel {
private:
    std::vector<Quarto*> _quartos;

    Quarto *get_quarto_pelo_n(int n_quarto);
public:
    Hotel();
    ~Hotel();

    std::vector<Quarto*> *get_quartos();
    void adiciona_reserva(int n_quarto, struct std::tm data_entrada, struct std::tm data_saida);
    void remove_reserva(int n_quarto, struct std::tm data_entrada);
    void print_info();
};

#endif