#include <iostream>
#include <sstream>
#include <set>
#include <vector>
#include <cctype>
#include <string>

int main() {
    std::set<std::string> entrada;
    std::string linha;

    while (std::getline(std::cin, linha)) {
        std::string palavra;
        std::stringstream linha_continua(linha);

        while (linha_continua >> palavra) {
            std::string palavra_minuscula;

            for (size_t i = 0; i < palavra.size(); ++i) {
                char c = palavra[i];

                if (std::isalpha(c)) {
                    palavra_minuscula += std::tolower(c);
                } else if (!palavra_minuscula.empty()) {
                    entrada.insert(palavra_minuscula);
                    palavra_minuscula.clear();
                }
            }

            if (!palavra_minuscula.empty()) {
                entrada.insert(palavra_minuscula);
            }
        }
    }

    std::vector<std::string> palavras_vector(palavras.begin(), palavras.end());

    for (size_t i = 0; i < palavras_vector.size(); i++) {
        std::cout << palavras_vector[i] << std::endl;
    }

    return 0;
}
