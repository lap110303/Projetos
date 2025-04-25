#ifndef SET_HPP
#define SET_HPP

#include <bits/stdc++.h>
#include <vector>
using namespace std;

typedef struct s_table_elem{
    string dado;
    bool vazio;
    bool retirada;

} ElementoTabela;

class StringSet{

    public:
        StringSet(int tamanho);
        ~StringSet();

        void Inserir(string s);
        void Remover(string s);
        bool Pertence(string s);
        
        StringSet* Intersecao(StringSet* S);
        StringSet* Uniao(StringSet* S);
        StringSet* DiferencaSimetrica(StringSet* S);

        void Imprimir();

    private:
        int tamanhoOriginal;
        int tamanhoTabela;
        int tamanhoConjunto;
        std::vector<string> ordemInsercao;
        ElementoTabela* tabela;

        int Hash(string s);
        void Rehash(int pos);
        void Resize(size_t tamanho);
};


#endif