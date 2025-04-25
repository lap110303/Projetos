#include <iostream>
#include <list>
#include <memory>
#include <string>
#include "Passageiro.hpp"
#include "Trem.hpp"
#include "Metro.hpp"
#include "Onibus.hpp"

using namespace std;

int main() {
    list<shared_ptr<Passageiro>> l_passageiro;
    unique_ptr<Trem> trem = nullptr;
    unique_ptr<Metro> metro = nullptr;
    unique_ptr<Onibus> onibus = nullptr;

    string comando;
    while (cin >> comando) {
        if (comando == "add") {
            string nome;
            int idade;
            float saldo;
            cin >> nome >> idade >> saldo;
            l_passageiro.push_back(make_shared<Passageiro>(nome, idade, saldo));
        } else if (comando == "trem") {
            int capacidade;
            float valor;
            cin >> capacidade >> valor;
            trem = unique_ptr<Trem>(new Trem(capacidade, valor));
            for (auto& p : l_passageiro) {
                trem->embarcar(p);
            }
            trem->mover();
            trem->desembarcar();
        } else if (comando == "metro") {
            int capacidade;
            float valor;
            cin >> capacidade >> valor;
            metro = unique_ptr<Metro>(new Metro(capacidade, valor));
            for (auto& p : l_passageiro) {
                metro->embarcar(p);
            }
            metro->mover();
            metro->desembarcar();
        } else if (comando == "onibus") {
            int capacidade;
            float valor;
            cin >> capacidade >> valor;
            onibus = unique_ptr<Onibus>(new Onibus(capacidade, valor));
            for (auto& p : l_passageiro) {
                onibus->embarcar(p);
            }
            onibus->mover();
            onibus->desembarcar();
        }
    }

    return 0;
}