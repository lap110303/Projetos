#ifndef RETANGULO_H
#define RETANGULO_H
#include "Figura.hpp"


class Retangulo : public Figura {
private:
    double _x;
    double _y;
    double _largura;
    double _altura;

public:
    Retangulo(double x, double y, double largura, double altura);
    bool contem(Ponto p) override;
    ~Retangulo();
};

#endif
