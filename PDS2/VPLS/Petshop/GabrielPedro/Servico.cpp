#include "Servico.hpp"
#include "Ave.hpp"
#include "Cachorro.hpp"
#include "Gato.hpp"
#include <cmath>
#include <iostream>
using namespace std;

Servico::Servico() = default;

Servico::~Servico() {
    for (auto orc : _orcamentos) {
        delete orc;
    }
}

void Servico::calcular_orcamento(Animal* animal) {
    float valor = 0.0;
    if (animal->get_subtipo() == "Ave") {
        Ave* ave = dynamic_cast<Ave*>(animal);
        valor = 30 * sqrt(ave->get_tamanho()) + 70;
    } else if (animal->get_subtipo() == "Cachorro") {
        Cachorro* cachorro = dynamic_cast<Cachorro*>(animal);
        valor = 64 * log1p(cachorro->get_peso()) + 15 * cachorro->get_tipo_pelo() + 100;
    } else if (animal->get_subtipo() == "Gato") {
        Gato* gato = dynamic_cast<Gato*>(animal);
        valor = 30 * gato->get_tipo_pelo() + 20 * gato->get_agressivo() + 150;
    }
    _orcamentos.push_back(new Orcamento(animal, valor));
}

void Servico::print_orcamentos() {
    for (auto orc : _orcamentos) {
        orc->print_info();
    }
}

void Servico::remove_orcamento(int id_animal) {
    for (auto it = _orcamentos.begin(); it != _orcamentos.end(); ++it) {
        if ((*it)->get_id_animal() == id_animal) {
            delete *it;
            _orcamentos.erase(it);
            return;
        }
    }
    cout << "O id " << id_animal << " nao faz parte da lista" << endl;
}
