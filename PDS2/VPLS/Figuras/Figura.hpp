#ifndef FIGURA_H
#define FIGURA_H
#include "Ponto.hpp"


class Figura {
public:
    virtual bool contem(Ponto p) = 0;
    virtual ~Figura();
    friend Figura* operator+(Figura& f1, Figura& f2);
    friend Figura* operator-(Figura& f1, Figura& f2);
};

class FiguraComposta : public Figura {
private:
    Figura* _f1;
    Figura* _f2;
    char _tipo;

public:
    FiguraComposta(Figura* f1, Figura* f2, char tipo);
    bool contem(Ponto p) override;
    ~FiguraComposta();
};

#endif
