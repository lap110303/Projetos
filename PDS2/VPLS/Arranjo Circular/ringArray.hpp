#ifndef RINGARRAY_H
#define RINGARRAY_H

#include <vector>
#include <iostream>

template <class T>
class RingArray {
private:
    std::vector<T> _buffer;
    int _capacidade;
    int _tamanho;
    int _head;
    int _tail;

public:
    RingArray(int capacidade) : _capacidade(capacidade), _tamanho(0), _head(0), _tail(0) {
        _buffer.resize(_capacidade);
    }

    void push(T item) {
        if (is_full()) {
            _buffer[_head] = item;
            _head = (_head + 1) % _capacidade;
            _tail = (_tail + 1) % _capacidade;
        } else{
            _buffer[_tail] = item;
            _tail = (_tail + 1) % _capacidade;
            if (_tamanho < _capacidade) {
                _tamanho++;
            }
        }
    }


    T get(int index) {
        int posicao = (_head + index) % _capacidade;
        return _buffer[posicao];
    }


    T pop() {
        if (is_empty()) {
            throw std::runtime_error("RingArray is empty");
        }
        T item = _buffer[_head];
        _head = (_head + 1) % _capacidade;
        _tamanho--;
        return item;
    }


    void print() {
        for (int i = 0; i < _tamanho; i++) {
            int posicao = (_head + i) % _capacidade;
            std::cout << _buffer[posicao] << std::endl;
        }
    }


    T get_maior() {

        T maior = _buffer[_head];
        int posicao = _head;

        for (int i = 1; i < _tamanho; i++) {
            posicao = (posicao + 1) % _capacidade;
            if (_buffer[posicao] > maior) {
                maior = _buffer[posicao];
            }
        }

        return maior;
    }



    int size() {
        return _tamanho;
    }


    bool is_empty() {
        return _tamanho == 0;
    }


    bool is_full() {
        return _tamanho == _capacidade;
    }
};

#endif
