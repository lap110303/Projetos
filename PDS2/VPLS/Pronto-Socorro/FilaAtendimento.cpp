#include "FilaAtendimento.hpp"

#include<iostream>

FilaAtendimento::FilaAtendimento() : head(nullptr), tail(nullptr), proxima_senha(1) {}

void FilaAtendimento::adicionar_paciente(int _prioridade, std::string _nome, int _idade) {
    
    Paciente* novo_paciente = new Paciente(_prioridade, _nome, _idade, proxima_senha++);
    Node* novo_node = new Node{.paciente = novo_paciente, .next = nullptr};
    
    if (head == nullptr) {
        head = novo_node;
        tail = novo_node;
    } else {
        Node* current = head;
        Node* prev = nullptr;

        // Encontre a posição correta para inserção com base na prioridade
        while (current != nullptr && current->paciente->getPrioridade() <= _prioridade) {
            prev = current;
            current = current->next;
        }

        // Insira o novo paciente na posição correta
        if (prev == nullptr) {
            novo_node->next = head;
            head = novo_node;
        } else {
            prev->next = novo_node;
            novo_node->next = current;
        }
    }
}

Paciente* FilaAtendimento::chamar_paciente_por_prioridade() {
    
    if (head == nullptr) {
        std::cout << "Nenhum paciente!" << std::endl;
        return nullptr;
    }

    Node* paciente_node = head;
    Paciente* paciente = paciente_node->paciente;
    head = head->next;

    return paciente;
}


Paciente* FilaAtendimento::chamar_paciente_por_idade() {
    
    Paciente* paciente_velho = buscar_paciente_mais_velho();
    if (paciente_velho == nullptr) {
        return nullptr;
    }

    Node* current = head;
    Node* prev = nullptr;

    while (current != nullptr && current->paciente->getID() != paciente_velho -> getID()) {
        prev = current;
        current = current->next;
    }

    if (prev != nullptr) {
        prev->next = current->next;
    } else {
        head = current->next;
    }
    Paciente* paciente_tirado = (current->paciente);
    delete current;

    return paciente_tirado;
}

Paciente* FilaAtendimento::buscar_paciente_mais_velho() {
    
    if (head == nullptr) {
        std::cout << "Nenhum paciente!" << std::endl;
        return nullptr;
    }

    Node* current = head;
    Node* node_velho = head;
    int mais_velho = head->paciente->getIdade();

    while (current != nullptr) {
        if (current->paciente->getIdade() > mais_velho) {
            mais_velho = current->paciente->getIdade();
            node_velho = current;
        }
        current = current->next;
    }

    Paciente* paciente_velho = node_velho->paciente;

    return paciente_velho;
}

void FilaAtendimento::estimativa_tempo_para_atendimento(int senha) {
    Node* current = head;
    int tempo_espera = 0;

    while (current != nullptr) {
        if (current->paciente->getID() == senha) {
            std::cout << "Tempo estimado para atendimento de " << current->paciente->getNome() << " (senha=" << senha << ") eh " << tempo_espera << " minutos." << std::endl;
            return;
        }
        tempo_espera += current->paciente->tempo_reservado_atendimento();
        current = current->next;
    }

    std::cout << "Paciente nao esta na fila!" << std::endl;
}

void FilaAtendimento::imprimir_fila() {
    Node* current = head;
    while (current != nullptr) {
        current->paciente->imprimir_dados();
        current = current->next;
    }
    if (head == nullptr) {
        std::cout << "Nenhum paciente!" << std::endl;
    }
}