#include "Carro.hpp"
#include<iostream>
#include<cmath>

Carro::Carro(double x, double y, double th) : x(x), y(y), th(th) {}

double Carro::getX() {
    return this->x;
}

double Carro::getY() {
    return this->y;
}

double Carro::getTH() {
    return this->th;
}

void Carro::girar(double w, double t) {
    this->th += w * t;
}

void Carro::mover(double v, double t) {
    double dx = v * std::cos(th) * t;
    double dy = v * std::sin(th) * t;
    this->x = x + dx;
    this-> y = y + dy;
}

double Carro::calcular_angulo_pessoa(Pessoa* pessoa) {
    double angulox = pessoa->getX() - this->x;
    double anguloy = pessoa->getY() - this->y;
    return std::atan2(anguloy, angulox) - this->th;
}

double Carro::calcular_distancia_pessoa(Pessoa* pessoa) {
    double angulox = pessoa->getX() - this->x;
    double anguloy = pessoa->getY() - this->y;
    return std::sqrt(angulox * angulox + anguloy * anguloy);
}

bool Carro::movimentacao_permitida(Pessoa** pessoas, int n) {
    bool p = true;
    for (int i = 0; i < n; ++i) {
        double angulo = calcular_angulo_pessoa(pessoas[i]) * 180.0 / M_PI;
        double distancia = calcular_distancia_pessoa(pessoas[i]);
        if (angulo >= -45.0 && angulo <= 45.0 && distancia < 5.0) {
            p = false;
            std::cout << "Alerta! " << i << "\t" << angulo << "\t" << distancia << std::endl;
        }
    }
    return p;
}

void Carro::navegar(Pessoa** pessoas, int n, double w, double v, double t) {
    if (!movimentacao_permitida(pessoas, n)) {
        return;
    }
    girar(w, t);
    mover(v, t);
}