#ifndef QUADTREE_HPP
#define QUADTREE_HPP

#include <string>

// Estrutura que representa uma estação de recarga
struct Estacao {
    std::string idend;
    long id_logrado;
    std::string sigla_tipo;
    std::string nome_logra;
    int numero_imo;
    std::string nome_bairr;
    std::string nome_regio;
    int cep;
    double x;
    double y;
    bool ativo;
};

// Nó da QuadTree, que contém as coordenadas e uma referência para uma estação
struct QuadTreeNode {
    double x, y;
    QuadTreeNode *NE, *NW, *SE, *SW;
    Estacao *estacao;

    QuadTreeNode(double x, double y, Estacao *estacao);
};

// Classe que representa a QuadTree
class QuadTree {
public:
    QuadTreeNode *raiz;

    QuadTree(); // Construtor
    void inserir(double x, double y, Estacao *estacao); // Insere uma nova estação

private:
    // Função de inserção recursiva
    void inserir(QuadTreeNode *&no, double x, double y, Estacao *estacao);
};

// Estrutura que associa uma estação a uma distância calculada
struct EstacaoDist {
    Estacao *estacao;
    double dist;
};

// Classe que implementa um vetor dinâmico para armazenar objetos EstacaoDist
class EstacaoDistVetor {
private:
    EstacaoDist* dado;
    std::size_t tamanho;
    std::size_t capacidade;

public:
    EstacaoDistVetor(); // Construtor
    ~EstacaoDistVetor(); // Destrutor
    void push_back(const EstacaoDist& value); // Adiciona no fim
    EstacaoDist* comeco(); // Primeiro elemento
    EstacaoDist* fim(); // Último elemento
    std::size_t get_tamanho() const; // Tamanho do vetor
    EstacaoDist& operator[](std::size_t index); // Acessos no vetor

private:
    void reinicia(); // Redimensiona o vetor
};

// Função de comparação para ordenar estações com base na distância
int comparar_estacoes(const void *a, const void *b);

// Função para consultar as estações mais próximas de um ponto (x, y) na QuadTree
void consulta_proximos(QuadTreeNode *no, double x, double y, EstacaoDistVetor &resultado);

// Função para destruir a QuadTree e liberar a memória
void destruir_quadtree(QuadTreeNode *no);

// Função para buscar uma estação na QuadTree pelo seu identificador
Estacao* buscar_estacao_por_id(QuadTreeNode *no, const std::string &id);

// Função para carregar uma estação a partir de uma linha de dados
Estacao* carregar_estacao(const std::string &linha);

// Função para processar eventos de ativação/desativação de estações e consultas
void processar_eventos(const std::string &nome_arquivo_eventos, QuadTreeNode *qt_root);

#endif
