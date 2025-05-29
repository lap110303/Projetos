#include "PokemonEletrico.hpp"

PokemonEletrico::PokemonEletrico(std::string nome, int level, double dano_ataque, double energia, double defesa, std::string ataque_basico, std::string ataque_eletrico):Pokemon(nome, "eletrico", level, dano_ataque, energia, defesa, "fisico"), _ataque_basico(ataque_basico), _ataque_eletrico(ataque_eletrico){}

PokemonEletrico::~PokemonEletrico(){}

std::string PokemonEletrico::getAtaqueBasico(){
    return this->_ataque_basico;
}


std::string PokemonEletrico::getAtaqueEletrico(){
    return this->_ataque_eletrico;
}


void PokemonEletrico::receberDano(double dano, std::string tipo){
    double energia = this->getEnergia();
    if(tipo == this->getFraqueza()){
        //Talvez arrumar o multiplicador da fraqueza
        energia -= dano*2;
    }else{
        energia -= dano;
    }
    this->setEnergia(energia);
}

double PokemonEletrico::calcularDanoAtaque(std::string ataque){
    if(ataque == this->getAtaqueBasico()){
        //Mudar pra _dano_basico talvez quando decidir o dano basico
        return this->getDano()*this->getDanoBasico();
    }
    else{
        //Mudar pra _dano_eletrico talvez quando decidir o dano eletrico
        return this->getDano()*this->getDanoEletrico();
    }
}

double PokemonEletrico::getDanoBasico(){
    return 3.0;
}


double PokemonEletrico::getDanoEletrico(){
    return 10.0;
}