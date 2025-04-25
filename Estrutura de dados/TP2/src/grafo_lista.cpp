#include <iostream>
#include <limits>
#include <cmath>
#include <stdexcept>
#include "fila_prioridade.hpp"
#include "grafo_lista.hpp"

GrafoLista::GrafoLista(int n) : tamanho(n) {
    //inicializador.
    try{
        nodes = new Node[n];
        for (int i = 0; i < n; ++i) {
            nodes[i].arestas = nullptr;
            nodes[i].portais = nullptr;
        }
    } catch (const std::bad_alloc& e) { //Caso haja algum erro, retorna a mensagem.
        throw std::runtime_error("Falha na alocação de memória para o grafo.");
    }
}

GrafoLista::~GrafoLista() { 
    //Destrutor.
    for (int i = 0; i < tamanho; ++i) {
        Aresta* aresta = nodes[i].arestas;
        while (aresta) {
            Aresta* temp = aresta;
            aresta = aresta->next;
            delete temp;
        }
        Portal* portal = nodes[i].portais;
        while (portal) {
            Portal* temp = portal;
            portal = portal->next;
            delete temp;
        }
    }
    delete[] nodes;
}

void GrafoLista::novaAresta(int w, int z, double peso) {
    // Caso haja erro retorna a mensagem.
    if (w < 0 || w >= tamanho || z < 0 || z >= tamanho) {
        throw std::out_of_range("Índice de vértice fora dos limites.");
    }
    // Cria uma nova aresta.
    Aresta* aresta = new Aresta{z, peso, nodes[w].arestas};
    nodes[w].arestas = aresta;
}

void GrafoLista::novoPortal(int w, int z) {
    // Caso haja erro retorna a mensagem.
    if (w < 0 || w >= tamanho || z < 0 || z >= tamanho) {
        throw std::out_of_range("Índice de vértice fora dos limites.");
    }
    // Cria um novo portal..
    Portal* portal = new Portal{z, nodes[w].portais};
    nodes[w].portais = portal;
}

void GrafoLista::setCoordenadas(int w, double x, double y) {
    // Define as coordenadas.
    nodes[w].x = x;
    nodes[w].y = y;
}

int GrafoLista::getTamanho() {
    // Retorna o número de vértices.
    return tamanho;
}

double GrafoLista::getCoordenadaX(int w) {
    //Retorna X de um vértice.
    return nodes[w].x;
}

double GrafoLista::getCoordenadaY(int w) {
    // Retorna Y de um vértice.
    return nodes[w].y;
}

Aresta* GrafoLista::getAresta(int w) {
    // Retorna as arestas que saem de um vértice.
    return nodes[w].arestas;
}

Portal* GrafoLista::getPortais(int w) {
    // Retorna os portais que saem de um vértice.
    return nodes[w].portais;
}

double heuristicaLista(double x0, double y0, double x1, double y1) {
    // Calcula a distância euclidiana que vai da origem ao destino.
    return std::sqrt(std::pow(x0 - x1, 2) + std::pow(y0 - y1, 2));
}

int dijkstraL(GrafoLista& grafo, int comeco, int fim, double s, int q) {
    int n = grafo.getTamanho();
    float* distancia = new float[n];
    int* portalUtilizado = new int[n];

    // Inicializa todos as distâncias com valor infinito.
    for (int i = 0; i < n; ++i) {
        distancia[i] = std::numeric_limits<double>::infinity();
        portalUtilizado[i] = 0;
    }

    // Define a origem.
    FilaPrioridade fp;
    distancia[comeco] = 0;
    fp.adiciona(comeco, 0, 0);

    while (!fp.vazio()) {
        // Enquanto a fila de prioridade não estiver vazia, obtem a maior prioridade e vai removendo.
        NodeFilaPrioridade* node = fp.topo();
        int w = node->vertice;
        fp.retira();

        if (w == fim) {
            int resultado;
            // Verifica se a energia e os portais utilizados são suficientes..
            if (distancia[fim] <= s && portalUtilizado[fim] <= q) {
                resultado = 1;
            } else {
                resultado = 0;
            }
            // Limpa a memória.
            delete[] distancia;
            delete[] portalUtilizado;
            return resultado;
        }

        // Percorre todas as arestas vizinhas de w.
        for (Aresta* aresta = grafo.getAresta(w); aresta; aresta = aresta->next) {
            int z = aresta->destino;
            double peso = aresta->peso;
            if (distancia[w] + peso < distancia[z]) { // Verifica se a distância dos vértices é menor que a atual.
                distancia[z] = distancia[w] + peso; // Atualiza a distância.
                portalUtilizado[z] = portalUtilizado[w]; // Atualiza os portais utilizados.
                fp.adiciona(z, distancia[z], portalUtilizado[z]); // Adiciona a fila de prioridade.
            }
        }

        // Percorre os portais vizinhos de w.
        for (Portal* portal = grafo.getPortais(w); portal; portal = portal->next) {
            int z = portal->destino;
            if (distancia[w] < distancia[z] && portalUtilizado[w] + 1 <= q) { // Verifica se a distância é menor e se os portais não excederam.
                distancia[z] = distancia[w]; //Atualiza a distância.
                portalUtilizado[z] = portalUtilizado[w] + 1; // Atualiza os portais utilizados.
                fp.adiciona(z, distancia[z], portalUtilizado[z] + 1); // Adiciona a fila de prioridade.
            }
        }
    }

    // Limpa a memória.
    delete[] distancia;
    delete[] portalUtilizado;
    return 0;
}

int aEstrelaL(GrafoLista& grafo, int comeco, int fim, double s, int q) {
    int n = grafo.getTamanho();
    double* distancia = new double[n];
    int* portalUtilizado = new int[n];

    // Inicializa a distância como infinito e os portais utilizados como 0.
    for (int i = 0; i < n; ++i) {
        distancia[i] = std::numeric_limits<double>::infinity();
        portalUtilizado[i] = 0;
    }

    FilaPrioridade fp;
    distancia[comeco] = 0; // Origem.

    double endX = grafo.getCoordenadaX(fim); // Destino.
    double endY = grafo.getCoordenadaY(fim); // Destino.

    // Adiciona o vértice inicial na fila com a heurística.
    fp.adiciona(comeco, heuristicaLista(grafo.getCoordenadaX(comeco), grafo.getCoordenadaY(comeco), endX, endY), 0);

    while (!fp.vazio()) {
        // Enquanto a fila de prioridade não estiver vazia, obtem a maior prioridade e vai removendo.
        NodeFilaPrioridade* node = fp.topo();
        int w = node->vertice;
        fp.retira();

        if (w == fim) {
            int resultado;

            // Verifica se a energia e os portais utilizados são suficientes.
            if (distancia[fim] <= s && portalUtilizado[fim] <= q) {
                resultado = 1;
            } else {
                resultado = 0;
            }

            // Limpa a memória.
            delete[] distancia;
            delete[] portalUtilizado;
            return resultado;
        }

        // Percorre as arestas vizinhas a w.
        for (Aresta* aresta = grafo.getAresta(w); aresta; aresta = aresta->next) {
            int z = aresta->destino;
            double peso = aresta->peso;
            double new_dist = distancia[w] + peso; // Calcula a nova distância
            if (new_dist < distancia[z]) { // Caso a nova distância seja menor, atualiza.
                distancia[z] = new_dist;
                portalUtilizado[z] = portalUtilizado[w];
                fp.adiciona(z, new_dist + heuristicaLista(grafo.getCoordenadaX(z), grafo.getCoordenadaY(z), endX, endY), portalUtilizado[z]);
            }
        }

        // Percorre os portais vizinhos a w.
        for (Portal* portal = grafo.getPortais(w); portal; portal = portal->next) {
            int z = portal->destino;

            // Caso a distância seja menor, e possa utilizar algum portal, atualiza.
            if (distancia[w] < distancia[z] && portalUtilizado[w] + 1 <= q) {
                distancia[z] = distancia[w];
                portalUtilizado[z] = portalUtilizado[w] + 1;
                fp.adiciona(z, distancia[z] + heuristicaLista(grafo.getCoordenadaX(z), grafo.getCoordenadaY(z), endX, endY), portalUtilizado[z] + 1);
            }
        }
    }

    // Limpa a memória
    delete[] distancia;
    delete[] portalUtilizado;
    return 0;
}

void leitorLista(GrafoLista& grafo, int& n, int& m, int& k, double& s, int& q) {
    if (!(std::cin >> n >> m >> k)) { // Recebe o número de vértices, arestas e portais.
        throw std::runtime_error("Erro ao ler o número de vértices, arestas e portais.");
    }
    grafo = GrafoLista(n);

    for (int i = 0; i < n; ++i) {
        double x, y;
        if (!(std::cin >> x >> y)) { // Recebe as coordenadas dos vértices.
            throw std::runtime_error("Erro ao ler as coordenadas do vértice.");
        }
        grafo.setCoordenadas(i, x, y);
    }

    for (int i = 0; i < m; ++i) {
        int w, z;
        if (!(std::cin >> w >> z)) { // Recebe as arestas e suas origens e destinos
            throw std::runtime_error("Erro ao ler as arestas.");
        }
        double peso = heuristicaLista(grafo.getCoordenadaX(w), grafo.getCoordenadaY(w), grafo.getCoordenadaX(z), grafo.getCoordenadaY(z));
        grafo.novaAresta(w, z, peso);
    }

    for (int i = 0; i < k; ++i) {
        int w, z;
        if (!(std::cin >> w >> z)) { // Recebe os portais e suas origens e destinos.
            throw std::runtime_error("Erro ao ler os portais.");
        }
        grafo.novoPortal(w, z);
    }

    if (!(std::cin >> s >> q)) { // Recee a energia e número máximo de portais
        throw std::runtime_error("Erro ao ler a energia e o número máximo de portais.");
    }
}