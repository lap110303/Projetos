#include "Reserva.hpp"
#include <ctime>

Reserva::Reserva(struct std::tm data_entrada, struct std::tm data_saida) {
    struct std::tm enddate = {};
    enddate.tm_year = 2023 - 1900;
    enddate.tm_mon = 12 - 7;
    enddate.tm_mday = 1;
    time_t now = mktime(&enddate);

    time_t entrada = mktime(&data_entrada);
    if (entrada < now) {
        throw reserva_excp::data_entrada_no_passado(data_entrada);
    }
    if (mktime(&data_entrada) > mktime(&data_saida)) {
        throw reserva_excp::data_saida_antecede_entrada(data_entrada, data_saida);
    }
    if ((mktime(&data_saida) - mktime(&data_entrada)) < DIA_EM_SEGUNDOS) {
        throw reserva_excp::periodo_muito_curto(data_entrada, data_saida);
    }

    _data_entrada = data_entrada;
    _data_saida = data_saida;
}

struct std::tm* Reserva::get_data_entrada() { return &_data_entrada; }
struct std::tm* Reserva::get_data_saida() { return &_data_saida; }

int Reserva::comparar(Reserva& other) {
    // Passando o endereço das variáveis _data_saida e _data_entrada
    if (mktime(&_data_saida) < mktime(&other._data_entrada)) return -1;  // Corrigido
    if (mktime(&other._data_saida) < mktime(&_data_entrada)) return 1;  // Corrigido
    return 0;  // Se não houve nenhuma diferença, as reservas têm períodos sobrepostos
}

void Reserva::print_info() {
    std::cout << "\tEntrada: " << std::put_time(&_data_entrada, FORMATO_DATA)
              << ", Saida: " << std::put_time(&_data_saida, FORMATO_DATA) << std::endl;
}
