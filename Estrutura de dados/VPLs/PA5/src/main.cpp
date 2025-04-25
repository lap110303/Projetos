#include <bits/stdc++.h>
#include "../include/Set.hpp"

using namespace std;

StringSet* LeConjunto(){
    int n;
    string k;
    cin >> n;
    StringSet* s = new StringSet(n);
    for(int i = 0; i < n; i++){
        cin >> k;
        s->Inserir(k);
    }
    return s;
}


int main(int argc, char** argv){
    int n;
    char c;
    string t;

    cin >> n;
    StringSet* s = new StringSet(1);

    for(int i = 0; i < n; i++){
        cin >> c;
        switch (c)
        {
            case 'i':
                cin >> t;
                s->Inserir(t);
                break;
            case 'r':
                cin >> t;
                s->Remover(t);
                break;
            case 'n':
                cin >> t;
                if(s->Pertence(t))
                    cout << "O elemento " << t << "pertence ao conjunto!\n";
                else
                    cout << "O elemento " << t << "nao pertence ao conjunto!\n";
                break;
            case 'u':{
                cout << "A uniao dos conjuntos e: ";
                StringSet* l = LeConjunto();
                StringSet* u = s->Uniao(l);
                u->Imprimir();
                delete l;
                delete u;
                break;
            }
            case 't':{
                cout << "A intersecao dos conjuntos e: ";
                StringSet* l = LeConjunto();
                StringSet* t = s->Intersecao(l);
                t->Imprimir();
                delete l;
                delete t;
                break;
            }
            case 'd':{
                cout << "A diferenca simetrica dos conjuntos e: ";
                StringSet* l = LeConjunto();
                StringSet* d = s->DiferencaSimetrica(l);
                d->Imprimir();
                delete l;
                delete d;
                break;
            }
            case 'p':
                s->Imprimir();
            default:
                break;
        }
    }

    delete s;
    return 0;
}

