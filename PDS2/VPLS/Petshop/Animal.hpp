#ifndef ANIMAL_HPP
#define ANIMAL_HPP

#include <string>
#include <iostream>
using namespace std;

class Animal {
private:
    int _id;
    static int _prox_id;
    string _nome;
    string _subtipo;

public:
    Animal(string nome, string subtipo);
    virtual ~Animal();
    virtual void print_info();
    int get_id();
    string get_subtipo();
    string get_nome();
};

#endif
