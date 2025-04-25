#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "ForcaBruta.hpp"
#include "HeldKarp.hpp"
#include "VizinhoMaisProx.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    // Verifica se o nome do arquivo foi fornecido como argumento
    if (argc != 2) {
        cerr << "Uso: " << argv[0] << " <nome_arquivo_entrada>" << endl;
        return 1;
    }

    string nomeArquivo = argv[1];

    // Abre o arquivo de entrada
    ifstream arquivoEntrada(nomeArquivo);
    if (!arquivoEntrada.is_open()) {
        cerr << "Erro ao abrir o arquivo de entrada: " << nomeArquivo << endl;
        return 1;
    }

    // Leitura da estratégia desejada
    char estrategia;
    arquivoEntrada >> estrategia;

    // Leitura do número de vértices e arestas
    int V, E;
    arquivoEntrada >> V >> E;

    // Mapeamento dos nomes das cidades para índices
    unordered_map<string, int> mapaCidades;
    vector<string> cidades;

    // Inicializa a matriz de distâncias
    const int INF = 1000000000;
    vector<vector<int>> distancias(V, vector<int>(V, INF));
    for (int i = 0; i < V; i++) {
        distancias[i][i] = 0;
    }

    // Leitura das estradas: cada linha contém "cidade1 cidade2 peso"
    for (int i = 0; i < E; i++) {
        string cidade1, cidade2;
        int peso;
        arquivoEntrada >> cidade1 >> cidade2 >> peso;

        if (mapaCidades.find(cidade1) == mapaCidades.end()) {
            int indice = mapaCidades.size();
            mapaCidades[cidade1] = indice;
            cidades.push_back(cidade1);
        }
        if (mapaCidades.find(cidade2) == mapaCidades.end()) {
            int indice = mapaCidades.size();
            mapaCidades[cidade2] = indice;
            cidades.push_back(cidade2);
        }

        int idx1 = mapaCidades[cidade1];
        int idx2 = mapaCidades[cidade2];

        distancias[idx1][idx2] = peso;
        distancias[idx2][idx1] = peso;
    }

    // Fecha o arquivo de entrada
    arquivoEntrada.close();

    // Executa o algoritmo com base na estratégia lida
    if (estrategia == 'b') {
        ForcaBruta fb(V, cidades, distancias);
        fb.encontrarMelhorRota();
        cout << fb.getMenorCusto() << endl;
        vector<string> rota = fb.getMelhorRota();
        for (const string& cidade : rota) {
            cout << cidade << " ";
        }
        cout << endl;
    } else if (estrategia == 'd') {
        HeldKarp hk(V, distancias);
        hk.resolver();
        cout << hk.getMenorCusto() << endl;
        vector<string> rota = hk.getMelhorCaminho(cidades);
        for (const string& cidade : rota) {
            cout << cidade << " ";
        }
        cout << endl;
    } else if (estrategia == 'g') {
        VizinhoMaisProx g(V, cidades, distancias);
        g.resolver();
        cout << g.getCustoTotal() << endl;
        vector<string> rota = g.getRota();
        for (const string& cidade : rota) {
            cout << cidade << " ";
        }
        cout << endl;
    } else {
        cout << "Estratégia não implementada." << endl;
    }

    return 0;
}
