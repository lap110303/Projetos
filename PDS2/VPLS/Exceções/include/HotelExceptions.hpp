#ifndef HOTEL_EXCEPTIONS_HPP
#define HOTEL_EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <sstream>

namespace hotel_excp {

    class quarto_nao_existe : public std::exception {
        private:
            int _n_quarto;
            mutable std::string _mensagem;

        public:
            quarto_nao_existe(int n_quarto) : _n_quarto(n_quarto) {
                // Gerar a mensagem de erro durante a construção da exceção
                std::ostringstream oss;
                oss << "Numero do quarto informado nao pertence a colecao: quarto " << _n_quarto;
                _mensagem = oss.str();  // Armazenar a mensagem na variável membro
            }

            const char* what() const throw() override {
                return _mensagem.c_str();  // Retornar a mensagem armazenada
            }

            int get_n_quarto() {
                return _n_quarto;
            }
    };

    class quarto_nao_criado : public std::exception {
        private:
            int _n_quarto;
            mutable std::string _mensagem;

        public:
            quarto_nao_criado(int n_quarto) : _n_quarto(n_quarto) {
                // Gerar a mensagem de erro durante a construção da exceção
                std::ostringstream oss;
                oss << "Quarto ainda nao foi adicionado a lista: quarto " << _n_quarto;
                _mensagem = oss.str();  // Armazenar a mensagem na variável membro
            }

            const char* what() const throw() override {
                return _mensagem.c_str();  // Retornar a mensagem armazenada
            }

            int get_n_quarto() {
                return _n_quarto;
            }
    };

}

#endif
