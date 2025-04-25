#ifndef RESERVA_H
#define RESERVA_H

#define DIA_EM_SEGUNDOS 86400

#define FORMATO_DATA "%d/%m/%Y"

#include "ReservaExceptions.hpp"

#include <iostream>
#include <iomanip>

class Reserva {
private:
    struct std::tm _data_entrada = {};
    struct std::tm _data_saida = {};

public:
    Reserva(struct std::tm data_entrada, struct std::tm data_saida);

    struct std::tm *get_data_entrada();
    struct std::tm *get_data_saida();
    int comparar(Reserva& other);
    void print_info();
};

#endif