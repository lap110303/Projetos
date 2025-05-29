#include "Insignia.hpp"

Insignia::Insignia(std::string nome, std::string tipo): _nome(nome), _tipo(tipo){}

Insignia::~Insignia() {}

std::string Insignia::getNome() {
    return this->_nome;
}

std::string Insignia::getTipo() {
    return this->_tipo;
}

bool Insignia::getExiste(){
    return this ->_existe;
}
void Insignia::setExiste(){

}
