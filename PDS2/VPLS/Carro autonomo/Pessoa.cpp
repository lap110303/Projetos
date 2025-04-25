#include "Pessoa.hpp"

Pessoa::Pessoa(double x, double y) : x(x), y(y) {}

double Pessoa::getX() {
    return x;
}

double Pessoa::getY() {
    return y;
}

void Pessoa::mover(double dx, double dy) {
    x = x + dx;
    y = y + dy;
}