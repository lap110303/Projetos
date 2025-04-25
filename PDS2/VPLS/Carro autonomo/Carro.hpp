#ifndef CARRO_H
#define CARRO_H

#include "Pessoa.hpp"

struct Carro {

    Carro(double x, double y, double th);
    double x;
    double y;
    double th;
    double getX();
    double getY();
    double getTH();
    void girar(double w, double t);
    void mover(double v, double t);
    double calcular_angulo_pessoa(Pessoa* pessoa);
    double calcular_distancia_pessoa(Pessoa* pessoa);
    bool movimentacao_permitida(Pessoa** pessoas, int n);
    void navegar(Pessoa** pessoas, int n, double w, double v, double t);

};

#endif