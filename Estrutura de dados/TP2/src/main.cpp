#include <iostream>
//#include "grafo_lista.hpp"
#include "grafo_matriz.hpp"

int main() {
    int n, m, k;
    double s;
    int q;

    // Inicializa o grafo para listas.
    //GrafoLista grafoLista(0);

    // Inicializa o grafo para matrizes.
    GrafoMatriz grafoMatriz(0);

    // Recebe os dados para lista.
    //leitorLista(grafoLista, n, m, k, s, q);

    //Recebe os dados para matriz.
    leitorMatriz(grafoMatriz, n, m, k, s, q);

    // Calcula o resultado para lista.
    //int dijkstraResultadoL = dijkstraL(grafoLista, 0, n - 1, s, q);
    //int aEstrelaResultadoL = aEstrelaL(grafoLista, 0, n - 1, s, q);

    // Calcula o resultado para matriz.
    int dijkstraResultadoM = dijkstraM(grafoMatriz, 0, n - 1, s, q);
    int aEstrelaResultadoM = aEstrelaM(grafoMatriz, 0, n - 1, s, q);

    // Printa o resultado para lista.
    //std::cout << dijkstraResultadoL << " " << aEstrelaResultadoL << std::endl;

    // Printa o resultado para matriz
    std::cout << dijkstraResultadoM << " " << aEstrelaResultadoM << std::endl;

    return 0;
}