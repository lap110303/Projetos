#include "../include/Set.hpp"

StringSet::StringSet(int tamanho) {
    tamanhoOriginal = tamanho;
    tamanhoTabela = tamanho;
    tamanhoConjunto = 0;
    tabela = new ElementoTabela[tamanhoTabela];
    for (int i = 0; i < tamanhoTabela; i++) {
        tabela[i].vazio = true;
        tabela[i].retirada = false;
    }
}

StringSet::~StringSet() {
    delete[] tabela;
}

int StringSet::Hash(string s) {
    int hashValue = 0;
    for (char c : s) {
        hashValue = (hashValue * 31 + c) % tamanhoTabela;
    }
    return hashValue;
}

void StringSet::Rehash(int pos) {
    string dado = tabela[pos].dado;
    tabela[pos].vazio = true;
    tabela[pos].retirada = false;
    Inserir(dado);
}

void StringSet::Resize(size_t novoTamanho) {
    ElementoTabela* novaTabela = new ElementoTabela[novoTamanho];
    for (size_t i = 0; i < novoTamanho; i++) {
        novaTabela[i].vazio = true;
        novaTabela[i].retirada = false;
    }

    int antigoTamanho = tamanhoTabela;
    ElementoTabela* antigaTabela = tabela;

    tabela = novaTabela;
    tamanhoTabela = novoTamanho;
    tamanhoConjunto = 0;

    for (int i = 0; i < antigoTamanho; i++) {
        if (!antigaTabela[i].vazio && !antigaTabela[i].retirada) {
            Inserir(antigaTabela[i].dado);
        }
    }

    delete[] antigaTabela;
}

void StringSet::Inserir(string s) {
    if (tamanhoConjunto >= tamanhoTabela / 2) {
        Resize(tamanhoTabela * 2);
    }

    int pos = Hash(s);
    while (!tabela[pos].vazio && !tabela[pos].retirada) {
        if (tabela[pos].dado == s) {
            return; // Evita inserção duplicada
        }
        pos = (pos + 1) % tamanhoTabela;
    }

    tabela[pos].dado = s;
    tabela[pos].vazio = false;
    tabela[pos].retirada = false;
    tamanhoConjunto++;
}

void StringSet::Remover(string s) {
    int pos = Hash(s);
    while (!tabela[pos].vazio) {
        if (!tabela[pos].retirada && tabela[pos].dado == s) {
            tabela[pos].retirada = true;
            tamanhoConjunto--;
            return;
        }
        pos = (pos + 1) % tamanhoTabela;
    }
}

bool StringSet::Pertence(string s) {
    int pos = Hash(s);
    while (!tabela[pos].vazio) {
        if (!tabela[pos].retirada && tabela[pos].dado == s) {
            return true;
        }
        pos = (pos + 1) % tamanhoTabela;
    }
    return false;
}

StringSet* StringSet::Uniao(StringSet* S) {
    StringSet* result = new StringSet(tamanhoConjunto + S->tamanhoConjunto);

    for (int i = 0; i < tamanhoTabela; i++) {
        if (!tabela[i].vazio && !tabela[i].retirada) {
            result->Inserir(tabela[i].dado);
        }
    }

    for (int i = 0; i < S->tamanhoTabela; i++) {
        if (!S->tabela[i].vazio && !S->tabela[i].retirada) {
            result->Inserir(S->tabela[i].dado);
        }
    }

    return result;
}

StringSet* StringSet::Intersecao(StringSet* S) {
    StringSet* result = new StringSet(min(tamanhoConjunto, S->tamanhoConjunto));

    for (int i = 0; i < tamanhoTabela; i++) {
        if (!tabela[i].vazio && !tabela[i].retirada && S->Pertence(tabela[i].dado)) {
            result->Inserir(tabela[i].dado);
        }
    }

    return result;
}

StringSet* StringSet::DiferencaSimetrica(StringSet* S) {
    StringSet* result = new StringSet(tamanhoConjunto + S->tamanhoConjunto);

    for (int i = 0; i < tamanhoTabela; i++) {
        if (!tabela[i].vazio && !tabela[i].retirada && !S->Pertence(tabela[i].dado)) {
            result->Inserir(tabela[i].dado);
        }
    }

    for (int i = 0; i < S->tamanhoTabela; i++) {
        if (!S->tabela[i].vazio && !S->tabela[i].retirada && !Pertence(S->tabela[i].dado)) {
            result->Inserir(S->tabela[i].dado);
        }
    }

    return result;
}

void StringSet::Imprimir() {
    for (int i = 0; i < tamanhoTabela; i++) {
        if (!tabela[i].vazio && !tabela[i].retirada) {
            cout << tabela[i].dado << " ";
        }
    }
    cout << endl;
}
