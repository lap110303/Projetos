#ifndef LISTA_ENCADEADA_HPP
#define LISTA_ENCADEADA_HPP

class No {
public:
    int valor;
    No* proximo;

    No(int valor);
};

class ListaEncadeada {
public:
    ListaEncadeada();
    ~ListaEncadeada();

    void adicionarNo(int valor);
    bool existeNo(int valor) const;
    int tamanho() const;
    void imprimir() const;

    No* primeiro() const;

private:
    No* cabeca;
    int tamanhoLista;
};

#endif
