#include <iostream>
#include "pilha.hpp"
#include "item.hpp"

using namespace std;

void t1(){
    Pilha<int> pilha(10);
    pilha.push(0);
    pilha.push(1);
    pilha.push(2);
    pilha.push(3);
    pilha.print();
}

void t2(){
    Pilha<int> pilha(10);
    pilha.push(0);
    pilha.push(1);
    pilha.push(2);
    pilha.push(3);
    int aux;
    pilha.pop(aux);
    cout << "Pop: " << aux << endl;
    pilha.print();
}

void t3(){
    bool suc;
    Pilha<int> pilha(1);
    suc = pilha.push(-1);
    cout << "Push success: " << suc << endl;
    pilha.print();
    suc = pilha.push(100);
    cout << "Push success: " << suc << endl;
    pilha.print();
}

void t4(){
    bool suc;
    int aux;
    Pilha<int> pilha(1);
    pilha.push(-1);
    suc = pilha.pop(aux);
    cout << "Pop: " << aux << endl;
    cout << "Pop success: " << suc << endl;
    pilha.print();
    suc = pilha.pop(aux);
    cout << "Pop success: " << suc << endl;
    pilha.print();
}

void t5(){
    Pilha<float> p(4);
    float aux;
    p.push(-0.5);
    p.pop(aux);
    cout << "Pop: " << aux << endl;
    p.pop(aux);
    cout << "Pop: " << aux << endl;
    p.print();
    p.push(-5.0);
    p.push(0.5);
    p.push(4.0);
    p.push(-0.5);
    p.push(50);
    p.push(100);
    p.print();
    p.get_maior(aux);
    cout << "Maior: " << aux << endl;
    p.pop(aux);
    cout << "Pop: " << aux << endl;
    p.pop(aux);
    cout << "Pop: " << aux << endl;
    p.get_maior(aux);
    cout << "Maior: " << aux << endl;
}

void t6(){
    Pilha<Item> pilha(10);
    Item val(0, 2);
    pilha.push(Item(1,3));
    pilha.push(Item(2,100));
    pilha.push(Item(3,-100));
    pilha.push(Item(4,1000));

    pilha.pop(val);
    cout << val << endl;

    pilha.get_maior(val);
    cout << val << endl;
    
    pilha.print();
}

void t7(){
    Pilha<Item> pilha(10);
    Item val(0, 2);
    pilha.push(Item(1,3));
    pilha.push(Item(2,100));
    pilha.push(Item(3,-100));
    pilha.push(Item(4,1000));

    pilha.pop(val);
    cout << val << endl;

    pilha.get_maior(val);
    cout << val << endl;

    pilha.pop(val);
    cout << val << endl;
    pilha.pop(val);
    cout << val << endl;

    pilha.get_maior(val);
    cout << val << endl;
    
    pilha.print();
}


int main(){
    int i;
    cin >> i;

    switch (i)
    {
    case 1:
        t1();
        break;
    case 2:
        t2();
        break;
    case 3:
        t3();
        break;
    case 4:
        t4();
        break;
    case 5:
        t5();
        break;
    case 6:
        t6();
        break;
    case 7:
        t7();
        break;
    
    default:
        break;
    }
    return 0;
}

