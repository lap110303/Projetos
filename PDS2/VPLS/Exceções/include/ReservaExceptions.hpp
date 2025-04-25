#ifndef RESERVA_EXCEPTIONS_HPP
#define RESERVA_EXCEPTIONS_HPP

#include <exception>
#include <ctime>
#include <string>

namespace reserva_excp {

    class data_entrada_no_passado : public std::exception {
    private:
        struct std::tm _data_entrada;

    public:
        data_entrada_no_passado(struct std::tm data_entrada) : _data_entrada(data_entrada) {}

        const char* what() const throw() override {
            static std::string msg;
            char buffer[100];
            std::strftime(buffer, sizeof(buffer), "%d/%m/%Y", &_data_entrada);
            msg = "Data de entrada esta no passado: " + std::string(buffer);
            return msg.c_str();
        }

        struct std::tm* get_data_entrada() {
            return &_data_entrada;
        }
    };

    class data_saida_antecede_entrada : public std::exception {
    private:
        struct std::tm _data_entrada;
        struct std::tm _data_saida;

    public:
        data_saida_antecede_entrada(struct std::tm data_entrada, struct std::tm data_saida)
            : _data_entrada(data_entrada), _data_saida(data_saida) {}

        const char* what() const throw() override {
        static std::string msg;
        char buffer_entrada[100];
        char buffer_saida[100];
        
        // Formatar as datas de entrada e saída
        std::strftime(buffer_entrada, sizeof(buffer_entrada), "%d/%m/%Y", &_data_entrada);
        std::strftime(buffer_saida, sizeof(buffer_saida), "%d/%m/%Y", &_data_saida);
        
        // Corrigir a ordem das datas na mensagem
        msg = "Data saida nao pode ser menor que a data entrada: " + std::string(buffer_entrada) + ", " + std::string(buffer_saida);
        return msg.c_str();
    }

        struct std::tm* get_data_entrada() {
            return &_data_entrada;
        }

        struct std::tm* get_data_saida() {
            return &_data_saida;
        }
    };

    class periodo_muito_curto : public std::exception {
    private:
        struct std::tm _data_entrada;
        struct std::tm _data_saida;

    public:
        periodo_muito_curto(struct std::tm data_entrada, struct std::tm data_saida)
            : _data_entrada(data_entrada), _data_saida(data_saida) {}

        const char* what() const throw() override {
            static std::string msg;
            char buffer_entrada[100];
            char buffer_saida[100];
            
            // Formatar as datas de entrada e saída
            std::strftime(buffer_entrada, sizeof(buffer_entrada), "%d/%m/%Y", &_data_entrada);
            std::strftime(buffer_saida, sizeof(buffer_saida), "%d/%m/%Y", &_data_saida);
            
            // Criar a mensagem com as datas formatadas
            msg = "Reserva deve ter duracao minima de 1 dia: " + std::string(buffer_entrada) + ", " + std::string(buffer_saida);
            return msg.c_str();
        }

        struct std::tm* get_data_entrada() {
            return &_data_entrada;
        }

        struct std::tm* get_data_saida() {
            return &_data_saida;
        }
    };

}

#endif