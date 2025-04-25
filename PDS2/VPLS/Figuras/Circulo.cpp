#include "Circulo.hpp"
#include <iostream>
#include <cmath>
using namespace std;

Circulo::Circulo(double x, double y, double r) : _x(x), _y(y), _r(r) {}

bool Circulo::contem(Ponto p) {
    double distance = hypot(p.getX() - _x, p.getY() - _y);
    return distance <= _r;
}

Circulo::~Circulo() {
    std::cout << "Destrutor Circulo" << std::endl;
}
