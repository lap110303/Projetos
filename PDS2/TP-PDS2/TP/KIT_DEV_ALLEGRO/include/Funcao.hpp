
#ifndef FUNCAO
#define FUNCAO

#include <string>
#include <sstream>

/**
 * @brief Classe de exemplo para uma função genérica.
 * 
 * Esta classe fornece um método para converter um valor genérico em uma string.
 * 
 * @tparam T O tipo do valor a ser convertido.
 */
template <typename T>
class Funcao {
    public:
    /**
     * @brief Converte um valor em uma string personalizada.
     * 
     * @param value O valor a ser convertido.
     * @return std::string A representação em string do valor.
     */
    std::string to_string_custom(const T& value) {
        std::ostringstream oss;
        oss << value;
        return oss.str();
    }
};

#endif