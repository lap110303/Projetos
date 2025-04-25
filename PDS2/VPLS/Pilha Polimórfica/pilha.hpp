#ifndef PILHA_HPP
#define PILHA_HPP

#include <iostream>
#include "item.hpp"

template <class T>
class Pilha {
private:
    T* _arr;
    int _max_tam;
    int _top;
    int i;
    int j;

public:
    friend class Item;
    Pilha(int max_tam) : _max_tam(max_tam), _top(-1) {
        _arr = new T[max_tam];
    }


    ~Pilha() {
        delete[] _arr;
    }

    bool push(const T& novo_e) {
        if (_top + 1 >= _max_tam){
            return false;
        }
        i = ++_top;
        _arr[i] = novo_e;
        return true;
    }

    bool pop(T& valor) {
        if (_top < 0){
            return false;
        }
        j = _top--;
        valor = _arr[j];
        return true;
    }


    bool get_maior(T& valor) {
        valor = _arr[0];
        for (int i = 1; i <= _top; ++i) {
            if (_arr[i] > valor){
                valor = _arr[i];
            }
        }
        return true;
    }


    bool vazia() {
        return _top < 0;
    }

    void print() {
        for (int i = 0; i <= _top; ++i) {
            std::cout << _arr[i] << std::endl;
        }
    }
};

bool operator>(const Item& i1, const Item& i2) {
    if (i1._val > i2._val){
        return true;
    } else {
        return false;
    }
}

std::ostream& operator<<(std::ostream& out, const Item& i) {
    out << "(" << i._key << "," << i._val << ")";
    return out;
}

#endif