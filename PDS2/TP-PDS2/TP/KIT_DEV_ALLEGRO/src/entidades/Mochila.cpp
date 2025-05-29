#include "Mochila.hpp"
#include "Funcao.hpp"

Mochila::Mochila() {
    // Construtor
}

Mochila::~Mochila() {
    // Destrutor
}


void Mochila::informarItens() {
    std::cout << "Itens na mochila:" << std::endl;
    Funcao<double> objeto;
    for (const auto& item : _itens) {
        std::cout << item->getNome() << "   "<< objeto.to_string_custom(item->getEnergia()) << std::endl;
    }
}

void Mochila::informarInsignias() {
    std::cout << "Insignias na mochila:" << std::endl;
    for (const auto& insignia : _insignias) {
        std::cout << insignia->getNome() << "   " << insignia->getTipo() <<std::endl;
    }
}

void Mochila::adicionarItem(Item* item) {
    _itens.push_back(item);
}

void Mochila::adicionarInsignia(Insignia* insignia) {
    _insignias.push_back(insignia);
}

/*void Mochila::rmItem() {
    std::string item;
    std::cout << "Digite o nome do item a ser removido: ";
    std::cin >> item;
    //itens.remove(item);
    std::cout << "Item removido com sucesso!" << std::endl;
}*/

/*void Mochila::rmInsignia() {
    std::string insignia;
    std::cout << "Digite o nome da insignia a ser removida: ";
    std::cin >> insignia;
    //insignias.remove(insignia);
    std::cout << "Insignia removida com sucesso!" << std::endl;
}*/
