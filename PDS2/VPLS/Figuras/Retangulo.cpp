#include "Retangulo.hpp"
#include <iostream>
using namespace std;

Retangulo::Retangulo(double x, double y, double largura, double altura)
    : _x(x), _y(y), _largura(largura), _altura(altura) {}

bool Retangulo::contem(Ponto p) {
     return p.getX() >= _x && p.getX() <= (_x + _largura) &&
           p.getY() >= _y && p.getY() <= (_y + _altura);
}

Retangulo::~Retangulo() {
    std::cout << "Destrutor Retangulo" << std::endl;
}
