#include "Pokemon.hpp"
#include "Funcao.hpp"

Pokemon::Pokemon(std::string nome, std::string tipo, int level, double dano_ataque, double energia, double defesa, std::string fraqueza): _nome(nome), _tipo(tipo), _level(level), _dano_ataque(dano_ataque), _energia(energia), _defesa(defesa), _fraqueza(fraqueza){}

Pokemon::~Pokemon(){}

std::string Pokemon::getNome(){
    return _nome;
}

std::string Pokemon::getTipo(){
    return _tipo;
}

std::string Pokemon::imprimirInfos(){
    std::string info = "";
    Funcao<double> objeto; 
    info += "Nome:            " + _nome + "\n";
    info += "Tipo:            " + _tipo + "\n";
    info += "Level:           " + objeto.to_string_custom(_level) + "\n";
    info += "Dano de Ataque:  " + objeto.to_string_custom(_dano_ataque) + "\n";
    info += "Energia:         " + objeto.to_string_custom(_energia) + "\n";
    info += "Defesa:          " + objeto.to_string_custom(_defesa) + "\n";
    return info;
}

void Pokemon::atacar(Pokemon* defensor, std::string ataque){
        double s = calcularDanoAtaque(ataque);
        std::string _tipo = this->getTipo();
        defensor -> receberDano(s, _tipo);
        std::cout << s << std::endl;
}

void Pokemon::setEnergia(double energia){
    this->_energia = energia;
}

double Pokemon::getDano(){
    return this->_dano_ataque;
}

void Pokemon::setDano(double dano_ataque){
    this->_dano_ataque = dano_ataque;
}

double Pokemon::getDanoSofrido(){
    return this->_dano_sofrido;
}

void Pokemon::setDanoSofrido(double dano_sofrido){
    this-> _dano_sofrido = dano_sofrido;
}

std::string Pokemon::getFraqueza(){
    return this->_fraqueza;
}

double Pokemon::getEnergia(){
    return this->_energia;
}

