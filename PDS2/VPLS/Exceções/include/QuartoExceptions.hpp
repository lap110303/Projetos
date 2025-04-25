#ifndef QUARTO_EXCEPTIONS_HPP
#define QUARTO_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <sstream>
#include <iomanip>
#include <ctime>

namespace quarto_excp {

    class reserva_nao_encontrada : public std::exception {
    private:
        int _n_quarto;
        struct std::tm _data_inicio;

    public:
        reserva_nao_encontrada(int n_quarto, struct std::tm data_inicio)
            : _n_quarto(n_quarto), _data_inicio(data_inicio) {}

        const char* what() const throw() override {
            // Usando stringstream para formatar a mensagem de erro
            std::ostringstream oss;
            oss << "Reserva nao encontrada no quarto " << _n_quarto << ": "
                << std::put_time(&_data_inicio, "%d/%m/%Y");
            
            // Armazenar a mensagem gerada
            _mensagem = oss.str();
            return _mensagem.c_str();
        }

        int get_n_quarto() {
            return _n_quarto;
        }

        struct std::tm* get_data_entrada() {
            return &_data_inicio;
        }

    private:
        mutable std::string _mensagem; // Mensagem formatada
    };

    class reserva_indisponivel : public std::exception {
    private:
        struct std::tm _data_inicio;
        struct std::tm _data_saida;
        mutable std::string _mensagem; // Mensagem formatada

    public:
        reserva_indisponivel(struct std::tm data_inicio, struct std::tm data_saida)
            : _data_inicio(data_inicio), _data_saida(data_saida) {}

        const char* what() const throw() override {
            // Usando stringstream para formatar a mensagem de erro com as datas
            std::ostringstream oss;
            oss << "Existe uma reserva em alguns dos dias do periodo de "
                << std::put_time(&_data_inicio, "%d/%m/%Y") << " a "
                << std::put_time(&_data_saida, "%d/%m/%Y");

            // Armazenar a mensagem gerada
            _mensagem = oss.str();
            return _mensagem.c_str();
        }

        struct std::tm* get_data_entrada() {
            return &_data_inicio;
        }

        struct std::tm* get_data_saida() {
            return &_data_saida;
        }
    };

}

#endif
