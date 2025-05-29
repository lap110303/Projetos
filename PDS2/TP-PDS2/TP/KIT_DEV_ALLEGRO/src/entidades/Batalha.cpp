#include "Batalha.hpp"

Batalha::Batalha() {
    _turnos = 0;
    _vencedor = "";
    _perdedor = "";
}

Batalha::~Batalha() {
    // Here you can add any cleanup code if needed
}

std::string Batalha::getEquipes() {
    std::string result = "";
    for(auto const& pair: equipes) {
        result += pair.first + "\n";
    }
    return result;
}

bool Batalha::getTurno() {
    return _turnos;
}

void Batalha::setTurno(){
    if(_turnos == true){
        _turnos =false;
    }
    if(_turnos == false){
        _turnos = true;
    }
}


std::string Batalha::getVencedor() {
    return _vencedor;
}

std::string Batalha::getPerdedor() {
    return _perdedor;
}

void Batalha::imprimir_infos() {
    std::cout << "Turno: " << _turnos << std::endl;
    std::cout << "Vencedor: " << _vencedor << std::endl;
    std::cout << "Perdedor: " << _perdedor << std::endl;
}