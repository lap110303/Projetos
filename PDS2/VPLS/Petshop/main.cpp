#include "Ave.hpp"
#include "Cachorro.hpp"
#include "Gato.hpp"
#include "Servico.hpp"
#include <list>
#include <iostream>
#include <unordered_map>

using namespace std;

enum Comandos { ADD_A, ADD_C, ADD_G, RM, RM_ORCA, PRINT, CALC_ORCA, PRINT_ORCA, INVALIDO };

Comandos get_comando(const string& cmd) {
    static unordered_map<string, Comandos> comandos = {
        {"add_a", ADD_A},
        {"add_c", ADD_C},
        {"add_g", ADD_G},
        {"rm", RM},
        {"rm_orca", RM_ORCA},
        {"print", PRINT},
        {"calc_orca", CALC_ORCA},
        {"print_orca", PRINT_ORCA}
    };
    auto it = comandos.find(cmd);
    return (it != comandos.end()) ? it->second : INVALIDO;
}

void print_list(const list<Animal*>& animais) {
    for (auto animal : animais) {
        animal->print_info();
    }
}

void rm_id(list<Animal*>& animais, int id) {
    for (auto it = animais.begin(); it != animais.end(); ++it) {
        if ((*it)->get_id() == id) {
            delete *it;
            animais.erase(it);
            return;
        }
    }
}

int main() {
    list<Animal*> animais;
    Servico servico;

    string cmd;
    while (cin >> cmd) {
        switch (get_comando(cmd)) {
            case ADD_A: {
                string nome;
                int tamanho;
                cin >> nome >> tamanho;
                animais.push_back(new Ave(nome, tamanho));
                break;
            }
            case ADD_C: {
                string nome;
                float peso;
                int tipo_pelo;
                cin >> nome >> peso >> tipo_pelo;
                animais.push_back(new Cachorro(nome, peso, tipo_pelo));
                break;
            }
            case ADD_G: {
                string nome;
                int tipo_pelo, agressivo;
                cin >> nome >> tipo_pelo >> agressivo;
                animais.push_back(new Gato(nome, tipo_pelo, agressivo));
                break;
            }
            case RM: {
                int id;
                cin >> id;
                rm_id(animais, id);
                break;
            }
            case RM_ORCA: {
                int id;
                cin >> id;
                servico.remove_orcamento(id);
                break;
            }
            case PRINT: {
                print_list(animais);
                break;
            }
            case CALC_ORCA: {
                for (auto animal : animais) {
                    servico.calcular_orcamento(animal);
                }
                cout << "Quantidade de orçamentos: " << animais.size() << endl;
                break;
            }
            case PRINT_ORCA: {
                servico.print_orcamentos();
                break;
            }
            case INVALIDO:
            default: {
                cerr << "Comando inválido: " << cmd << endl;
                break;
            }
        }
    }

    for (auto animal : animais) {
        delete animal;
    }

    return 0;
}
