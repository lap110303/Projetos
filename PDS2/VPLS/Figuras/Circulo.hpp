#ifndef CIRCULO_H
#define CIRCULO_H
#include "Figura.hpp"


class Circulo : public Figura {
private:
    double _x;
    double _y;
    double _r;

public:
    Circulo(double x, double y, double r);
    bool contem(Ponto p) override;
    ~Circulo();
};

#endif
