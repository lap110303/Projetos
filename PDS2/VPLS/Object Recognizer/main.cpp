// NÃO ALTERE ESSA LINHA
#include "avaliacao_basica_recognizer.hpp"

#include <iostream>
#include <vector>
#include <string>
#include "Object.hpp"
#include "Dataset.hpp"
#include "Recognizer.hpp"

int main() {
    Recognizer recognizer;
    std::map<std::string, std::vector<std::string>> objectCommands;

    std::string letra;
    while (std::cin >> letra) {
        if (letra == "d") {
            std::string category;
            std::cin >> category;
            recognizer.add_dataset(category);
        }
        if (letra == "o") {
            std::string category, name;
            int vsize;
            std::cin >> category >> name >> vsize;
            std::vector<double> featureVector(vsize);
            for (int i = 0; i < vsize; ++i) {
                std::cin >> featureVector[i];
            }
            recognizer.add_object(category, name, featureVector);
        }
        if (letra == "m") {
            std::string category, name;
            std::cin >> category >> name;
            recognizer.remove_object(category, name);
        }
        if (letra == "p") {
            std::string category;
            std::cin >> category;
            recognizer.print_dataset_summary(category);
        }
        if (letra == "r") {
            std::string category;
            int vsize;
            std::cin >> category >> vsize;
            std::vector<double> inputFeature(vsize);
            for (int i = 0; i < vsize; ++i) {
                std::cin >> inputFeature[i];
            }
            recognizer.recognize_object(category, inputFeature);
        }
        if (letra == "b") {
            avaliacao_basica();
        }
    }

    return 0;
}