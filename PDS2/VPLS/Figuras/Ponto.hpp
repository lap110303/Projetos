#ifndef PONTO_H
#define PONTO_H
#include <iostream>
using namespace std;


class Ponto {
private:
    double _x;
    double _y;

public:
    Ponto(double x = 0, double y = 0) : _x(x), _y(y) {}

    double getX() {
        return _x;
    }

    double getY() {
        return _y;
    }
};

#endif