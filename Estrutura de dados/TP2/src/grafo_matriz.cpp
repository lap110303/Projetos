#include "grafo_matriz.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>

GrafoMatriz::GrafoMatriz(int n) : tamanho(n) {
    //inicializador.
    try{
        matriz = new double*[n];
        for (int i = 0; i < n; ++i) {
            matriz[i] = new double[n];
            for (int j = 0; j < n; ++j) {
                matriz[i][j] = std::numeric_limits<double>::infinity(); // Seta como infinito locais não visitados
            }
        }
        nodes = new Node[n];
        for (int i = 0; i < n; ++i) {
            nodes[i].portais = nullptr;
            nodes[i].contadorPortal = 0;
        }
    } catch (const std::bad_alloc& e) { // Em caso de erro retorna a mensagem
        throw std::runtime_error("Falha na alocação de memória para o grafo.");
    }
}

GrafoMatriz::~GrafoMatriz() {
    // Destrutor
    for (int i = 0; i < tamanho; ++i) {
        delete[] matriz[i];
        delete[] nodes[i].portais;
    }
    delete[] matriz;
    delete[] nodes;
}

void GrafoMatriz::novaAresta(int w, int z, double peso) {
    // Caso haja erro retorna a mensagem.
    if (w < 0 || w >= tamanho || z < 0 || z >= tamanho) {
        throw std::out_of_range("Índice de vértice fora dos limites.");
    }
    // Cria uma nova aresta.
    matriz[w][z] = peso;
}

void GrafoMatriz::novoPortal(int w, int z) {
    // Caso haja erro retorna a mensagem.
    if (w < 0 || w >= tamanho || z < 0 || z >= tamanho) {
        throw std::out_of_range("Índice de vértice fora dos limites.");
    }
    // Cria um novo portal e começa a contá-los.
    int newPortalCount = nodes[w].contadorPortal + 1;
    int* newPortals = new int[newPortalCount];
    for (int i = 0; i < nodes[w].contadorPortal; ++i) {
        newPortals[i] = nodes[w].portais[i];
    }
    newPortals[nodes[w].contadorPortal] = z;
    delete[] nodes[w].portais;
    nodes[w].portais = newPortals;
    nodes[w].contadorPortal = newPortalCount;
}

void GrafoMatriz::setCoordenadas(int w, double x, double y) {
    // Define as coordenadas.
    nodes[w].x = x;
    nodes[w].y = y;
}

int GrafoMatriz::getTamanho() {
    // Retorna o número de vértices.
    return tamanho;
}

double GrafoMatriz::getCoordenadaX(int w) {
    //Retorna X de um vértice.
    return nodes[w].x;
}

double GrafoMatriz::getCoordenadaY(int w) {
    // Retorna Y de um vértice.
    return nodes[w].y;
}

double GrafoMatriz::getArestaPeso(int w, int z) {
    // Retorna o peso de uma aresta que vai de w a z.
    return matriz[w][z];
}

int* GrafoMatriz::getPortais(int w, int& count) {
    // Retorna os portais que saem de um vértice e conta-os.
    count = nodes[w].contadorPortal;
    return nodes[w].portais;
}

double heuristicaMatriz(double x1, double y1, double x2, double y2) {
    // Calcula a distância euclidiana que vai da origem ao destino.
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}

int dijkstraM(GrafoMatriz& grafo, int comeco, int fim, double s, int q) {
    int n = grafo.getTamanho();
    double* distancia = new double[n];
    int* portalUtilizado = new int[n];

    // Inicializa todas as distâncias com valor infinito.
    for (int i = 0; i < n; ++i) {
        distancia[i] = std::numeric_limits<double>::infinity();
        portalUtilizado[i] = 0;
    }

    // Define a origem..
    FilaPrioridade fp;
    distancia[comeco] = 0;
    fp.adiciona(comeco, 0, 0);

    while (!fp.vazio()) {
        // Enquanto a fila de prioridade não estiver vazia, percorre os vizinhos e vai removendo.
        NodeFilaPrioridade* node = fp.topo();
        int w = node->vertice;
        fp.retira();

        if (w == fim) {
            int resultado;

            // Verifica se a energia e os portais utilizados são suficientes
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
        for (int z = 0; z < n; ++z) {
            double peso = grafo.getArestaPeso(w, z);
            if (peso < std::numeric_limits<double>::infinity() && distancia[w] + peso < distancia[z]) { // Verifica se a distância dos vértices é menor que a atual.
                distancia[z] = distancia[w] + peso; // Atualiza a distância.
                portalUtilizado[z] = portalUtilizado[w]; // Atualiza os portais utilizados.
                fp.adiciona(z, distancia[z], portalUtilizado[z]); // Adiciona a fila de prioridade.
            }
        }

        int contadorPortal;
        int* portais = grafo.getPortais(w, contadorPortal);

        // Percorre os portais vizinhos de w
        for (int i = 0; i < contadorPortal; ++i) {
            int z = portais[i];
            if (distancia[w] < distancia[z] && portalUtilizado[w] + 1 <= q) { // Verifica se a distância dos vértices é menor que a atual.
                distancia[z] = distancia[w]; // Atualiza a distância.
                portalUtilizado[z] = portalUtilizado[w] + 1; // Atualiza os portais utilizados.
                fp.adiciona(z, distancia[z], portalUtilizado[z] + 1); // Adiciona a fila de prioridade.
            }
        }
    }

    // Limpa a memória
    delete[] distancia;
    delete[] portalUtilizado;
    return 0;
}

int aEstrelaM(GrafoMatriz& grafo, int comeco, int fim, double s, int q) {
    int n = grafo.getTamanho();
    double* distancia = new double[n];
    int* portalUtilizado = new int[n];

    // Inicializa a distância como infinito e os portais utilizados como 0.
    for (int i = 0; i < n; ++i) {
        distancia[i] = std::numeric_limits<double>::infinity();
        portalUtilizado[i] = 0;
    }

    FilaPrioridade fp;
    distancia[comeco] = 0;// Origem.

    double endX = grafo.getCoordenadaX(fim); // Destino.
    double endY = grafo.getCoordenadaY(fim); // Destino.

    // Adiciona o vértice inicial na fila com a heurística.
    fp.adiciona(comeco, heuristicaMatriz(grafo.getCoordenadaX(comeco), grafo.getCoordenadaY(comeco), endX, endY), 0);

    while (!fp.vazio()) {
        // Enquanto a fila de prioridade não estiver vazia, obtem a maior prioridade e vai removendo.
        NodeFilaPrioridade* node = fp.topo();
        int w = node->vertice;
        fp.retira();

        if (w == fim) {
            int resultado;

            // Verifica se a energia e os portais utilizados são suficientes
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
        for (int z = 0; z < n; ++z) {
            double peso = grafo.getArestaPeso(w, z);
            if (peso < std::numeric_limits<double>::infinity()) { // Caso n tenha sido visitada, verifica a distancia e atualiza.
                double new_dist = distancia[w] + peso;
                if (new_dist < distancia[z]) {
                    distancia[z] = new_dist;
                    portalUtilizado[z] = portalUtilizado[w];
                    fp.adiciona(z, new_dist + heuristicaMatriz(grafo.getCoordenadaX(z), grafo.getCoordenadaY(z), endX, endY), portalUtilizado[z]);
                }
            }
        }

        int contadorPortal;
        int* portais = grafo.getPortais(w, contadorPortal);

        // Percorre os portais vizinhos a w.
        for (int i = 0; i < contadorPortal; ++i) {
            int z = portais[i];

            //Caso a distância seja menor, e possa utilizar algum portal, atualiza.
            if (distancia[w] < distancia[z] && portalUtilizado[w] + 1 <= q) {
                distancia[z] = distancia[w];
                portalUtilizado[z] = portalUtilizado[w] + 1;
                fp.adiciona(z, distancia[z] + heuristicaMatriz(grafo.getCoordenadaX(z), grafo.getCoordenadaY(z), endX, endY), portalUtilizado[z] + 1);
            }
        }
    }

    // Limpa a memória.
    delete[] distancia;
    delete[] portalUtilizado;
    return 0;
}

void leitorMatriz(GrafoMatriz& grafo, int& n, int& m, int& k, double& s, int& q) {
    if (!(std::cin >> n >> m >> k)) { // Recebe o número de vértices, arestas e portais.
        throw std::runtime_error("Erro ao ler o número de vértices, arestas e portais.");
    }
    grafo = GrafoMatriz(n);

    for (int i = 0; i < n; ++i) {
        double x, y;
        if (!(std::cin >> x >> y)) { // Recebe as coordenadas dos vértices.
            throw std::runtime_error("Erro ao ler as coordenadas do vértice.");
        }
        grafo.setCoordenadas(i, x, y);
    }

    for (int i = 0; m > 0 && i < m; ++i) {  // Recebe as arestas e suas origens e destinos
        int w, z;
        if (!(std::cin >> w >> z)) {
            throw std::runtime_error("Erro ao ler as arestas.");
        }
        double peso = heuristicaMatriz(grafo.getCoordenadaX(w), grafo.getCoordenadaY(w), grafo.getCoordenadaX(z), grafo.getCoordenadaY(z));
        grafo.novaAresta(w, z, peso);
    }

    for (int i = 0; k > 0 && i < k; ++i) {  // Recebe os portais e suas origens e destinos.
        int w, z;
        if (!(std::cin >> w >> z)) {
            throw std::runtime_error("Erro ao ler os portais.");
        }
        grafo.novoPortal(w, z);
    }

    if (!(std::cin >> s >> q)) { // Recee a energia e número máximo de portais
        throw std::runtime_error("Erro ao ler a energia e o número máximo de portais.");
    }
}
