#ifndef PESSOA_H
#define PESSOA_H

struct Pessoa {

    Pessoa(double x, double y);
    double x;
    double y;
    double getX();
    double getY();
    void mover(double dx, double dy);

};

#endif