#include "Item.hpp"

Item::Item(std::string nome, double energia, double ataque, double defesa): _nome(nome), modificador_energia(energia),
    modificador_ataque(ataque), modificador_defesa(defesa){}

Item::~Item(){}

std::string Item::getNome(){
    return this->_nome;
}

double Item::getEnergia(){
    return this-> modificador_energia;
}

void Item::modificarStatus(){

}

void Item::modificarEnergia(Pokemon* pokemon, double energia){
    energia += pokemon->getEnergia();
    pokemon->setEnergia(energia);
}

void Item::modificarDanoAtaque(Pokemon* pokemon, double ataque){
    ataque += pokemon->getDano();
    pokemon->setDano(ataque);
}

void Item::modificarDefesa(Pokemon* pokemon, double defesa){
    defesa += pokemon->getDanoSofrido();
    pokemon->setDanoSofrido(defesa);
}
