#include "QuadTree.hpp"
#include <cmath>
#include <iostream>
#include <iomanip>
#include <fstream>

// Construtor do nó da QuadTree
QuadTreeNode::QuadTreeNode(double x, double y, Estacao *estacao)
    : x(x), y(y), NE(nullptr), NW(nullptr), SE(nullptr), SW(nullptr), estacao(estacao) {}

// Construtor da QuadTree, inicializando a raiz como nula
QuadTree::QuadTree() : raiz(nullptr) {}

// Inserir uma estação na QuadTree
void QuadTree::inserir(double x, double y, Estacao *estacao) {
    inserir(raiz, x, y, estacao);  // Chama a função privada auxiliar
}

// Inserir um nó na posição correta da QuadTree
void QuadTree::inserir(QuadTreeNode *&no, double x, double y, Estacao *estacao) {
    if (no == nullptr) {
        no = new QuadTreeNode(x, y, estacao);  // Cria um novo nó se a posição estiver vazia
    } else {
        // Determina a direção e insere recursivamente
        if (x < no->x) {
            if (y < no->y)
                inserir(no->SW, x, y, estacao);
            else
                inserir(no->NW, x, y, estacao);
        } else {
            if (y < no->y)
                inserir(no->SE, x, y, estacao);
            else
                inserir(no->NE, x, y, estacao);
        }
    }
}

// Construtor da classe EstacaoDistVetor
EstacaoDistVetor::EstacaoDistVetor() : tamanho(0), capacidade(10) {
    dado = new EstacaoDist[capacidade];
}

// Destrutor para liberar a memória alocada dinamicamente
EstacaoDistVetor::~EstacaoDistVetor() {
    delete[] dado;
}

// Função para adicionar um novo elemento ao vetor
void EstacaoDistVetor::push_back(const EstacaoDist& value) {
    if (tamanho == capacidade) {
        reinicia();  // Redimensiona o vetor se a capacidade atual for insuficiente
    }
    dado[tamanho++] = value;
}

// Retorna um ponteiro para o primeiro elemento do vetor
EstacaoDist* EstacaoDistVetor::comeco() {
    return dado;
}

// Retorna um ponteiro para o último elemento do vetor
EstacaoDist* EstacaoDistVetor::fim() {
    return dado + tamanho;
}

// Retorna o tamanho atual do vetor
std::size_t EstacaoDistVetor::get_tamanho() const {
    return tamanho;
}

// Operador de índice para acessar os elementos do vetor
EstacaoDist& EstacaoDistVetor::operator[](std::size_t index) {
    return dado[index];
}

// Função para redimensionar o vetor
void EstacaoDistVetor::reinicia() {
    capacidade *= 2;  // Duplica a capacidade
    EstacaoDist* novo_dado = new EstacaoDist[capacidade];
    // Copia os elementos existentes para o novo array
    for (std::size_t i = 0; i < tamanho; i++) {
        novo_dado[i] = dado[i];
    }
    delete[] dado;  // Libera a memória antiga
    dado = novo_dado;  // Atualiza o ponteiro para o novo array
}

// Ordenar as estações por distância
int comparar_estacoes(const void *a, const void *b) {
    EstacaoDist *dist_a = (EstacaoDist *)a;
    EstacaoDist *dist_b = (EstacaoDist *)b;
    if (dist_a->dist < dist_b->dist) return -1;
    if (dist_a->dist > dist_b->dist) return 1;
    return 0;
}

// Encontra as estações mais próximas de um ponto (x, y)
void consulta_proximos(QuadTreeNode *no, double x, double y, EstacaoDistVetor &resultado) {
    if (no == nullptr) return;

    if (no->estacao->ativo) {
        // Calcula a distância da estação ao ponto de referência
        double dist = std::sqrt(std::pow(no->x - x, 2) + std::pow(no->y - y, 2));
        // Adiciona a estação ao vetor de resultados
        resultado.push_back({no->estacao, dist});
    }

    // Continua a busca nas quatro subárvores
    consulta_proximos(no->NE, x, y, resultado);
    consulta_proximos(no->NW, x, y, resultado);
    consulta_proximos(no->SE, x, y, resultado);
    consulta_proximos(no->SW, x, y, resultado);
}

// Função recursiva para destruir a QuadTree e liberar a memória
void destruir_quadtree(QuadTreeNode *no) {
    if (no == nullptr) return;
    destruir_quadtree(no->NE);
    destruir_quadtree(no->NW);
    destruir_quadtree(no->SE);
    destruir_quadtree(no->SW);
    delete no;
}

// Busca uma estação pelo seu identificador dentro da QuadTree
Estacao* buscar_estacao_por_id(QuadTreeNode *no, const std::string &id) {
    if (no == nullptr) return nullptr;

    if (no->estacao->idend == id) {
        return no->estacao;  // Retorna a estação se o identificador coincidir
    }

    // Continua a busca nas subárvores
    Estacao *resultado = buscar_estacao_por_id(no->NE, id);
    if (resultado == nullptr) resultado = buscar_estacao_por_id(no->NW, id);
    if (resultado == nullptr) resultado = buscar_estacao_por_id(no->SE, id);
    if (resultado == nullptr) resultado = buscar_estacao_por_id(no->SW, id);

    return resultado;
}

// Carrega os dados de uma estação a partir de uma linha de texto
Estacao* carregar_estacao(const std::string &linha) {
    Estacao* estacao = new Estacao;
    char idend[13], sigla_tipo[5], nome_logra[50], nome_bairr[50], nome_regio[20];

    // Extrai os campos da estação
    sscanf(linha.c_str(), "%12[^;];%ld;%4[^;];%49[^;];%d;%49[^;];%19[^;];%d;%lf;%lf",
           idend, &estacao->id_logrado, sigla_tipo, nome_logra,
           &estacao->numero_imo, nome_bairr, nome_regio, &estacao->cep,
           &estacao->x, &estacao->y);

    // Atribui os valores aos campos da estrutura
    estacao->idend = idend;
    estacao->sigla_tipo = sigla_tipo;
    estacao->nome_logra = nome_logra;
    estacao->nome_bairr = nome_bairr;
    estacao->nome_regio = nome_regio;
    estacao->ativo = true;  // Marca a estação como ativa

    return estacao;
}

// Processa eventos de consulta e ativação/desativação de estações
void processar_eventos(const std::string &nome_arquivo_eventos, QuadTreeNode *qt_root) {
    std::ifstream arquivo(nome_arquivo_eventos);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de eventos" << std::endl;
        return;
    }

    char comando;
    std::string id;
    double x, y;
    int n;

    // Processa cada linha do arquivo de eventos
    while (arquivo >> comando) {
        if (comando == 'C') {
            // Evento de consulta de estações próximas
            arquivo >> x >> y >> n;
            std::cout << std::fixed << std::setprecision(6);
            std::cout << "C " << x << " " << y << " " << n << std::endl;

            EstacaoDistVetor resultado;
            consulta_proximos(qt_root, x, y, resultado);

            // Ordena os resultados pela distância
            std::qsort(resultado.comeco(), resultado.get_tamanho(), sizeof(EstacaoDist), comparar_estacoes);

            // Imprime as n estações mais próximas
            for (std::size_t i = 0; i < static_cast<std::size_t>(n) && i < resultado.get_tamanho(); i++) {
                std::cout << resultado[i].estacao->sigla_tipo << " " 
                          << resultado[i].estacao->nome_logra << ", " 
                          << resultado[i].estacao->numero_imo << ", " 
                          << resultado[i].estacao->nome_bairr << ", " 
                          << resultado[i].estacao->nome_regio << ", " 
                          << resultado[i].estacao->cep << " (" 
                          << std::fixed << std::setprecision(3) << resultado[i].dist << ")" 
                          << std::endl;
            }
        } else if (comando == 'A' || comando == 'D') {
            // Eventos de ativação/desativação de estações
            arquivo >> id;
            Estacao *estacao = buscar_estacao_por_id(qt_root, id);
            if (estacao == nullptr) continue;

            if (comando == 'A') {
                // Ativação da estação
                if (estacao->ativo) {
                    std::cout << "A " << id << std::endl;
                    std::cout << "Ponto de recarga " << id << " já estava ativo." << std::endl;
                } else {
                    estacao->ativo = true;
                    std::cout << "A " << id << std::endl;
                    std::cout << "Ponto de recarga " << id << " ativado." << std::endl;
                }
            } else if (comando == 'D') {
                // Desativação da estação
                if (!estacao->ativo) {
                    std::cout << "D " << id << std::endl;
                    std::cout << "Ponto de recarga " << id << " já estava desativado." << std::endl;
                } else {
                    estacao->ativo = false;
                    std::cout << "D " << id << std::endl;
                    std::cout << "Ponto de recarga " << id << " desativado." << std::endl;
                }
            }
        }
    }
}
