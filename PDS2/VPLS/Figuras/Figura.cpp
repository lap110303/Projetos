#include "Figura.hpp"
#include <iostream>
using namespace std;

Figura::~Figura() {
    std::cout << "Destrutor Figura" << std::endl;
}

Figura* operator+(Figura& f1, Figura& f2) {
     return new FiguraComposta(&f1, &f2, '+');
}

Figura* operator-(Figura& f1, Figura& f2) {
     return new FiguraComposta(&f1, &f2, '-');
}

FiguraComposta::FiguraComposta(Figura* f1, Figura* f2, char tipo) : _f1(f1), _f2(f2), _tipo(tipo) {}

bool FiguraComposta::contem(Ponto p) {
    if (_tipo == '+') {
        return _f1->contem(p) || _f2->contem(p);
    }
    if (_tipo == '-') {
        return _f1->contem(p) && !_f2->contem(p);
    }
    return false;
}

FiguraComposta::~FiguraComposta() {
    std::cout << "Destrutor FiguraComposta" << std::endl;
}
