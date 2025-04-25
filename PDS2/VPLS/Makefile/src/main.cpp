#include <iostream>

#include "Heroi.hpp"
#include "Vilao.hpp"

using namespace std;

/*
DC:
Superman
Batman

PRINCIPAIS VILOES DC:
Lex Luthor - Livrar o planeta de um heroi perigoso
Coringa - Derrotar o Batman


MARVEL:
Homem Aranha
Homem de Ferro

PRINCIPAIS VILOES MARVEL E SEUS OBJETIVOS:
Marvel - Criar ilusoes e conseguir a tecnologia Stark
Justin Hammer - Roubar a tecnologia Stark e vender robos ao governo
*/

int main(){
    cout << "PDS2 - Makefile" << endl;

    //Definido heróis e vilões
    Vilao v1("Lex Luthor","DC", "livrar o planeta de um heroi perigoso");
    Heroi h1("Superman","DC");

    Vilao v2("Coringa","DC", "derrotar o Batman");
    Heroi h2("Batman","DC");

    Vilao v3("Mysterio","Marvel", "criar ilusoes e conseguir a tecnologia Stark");
    Heroi h3("Homem Aranha (Miranha)","Marvel");
    
    Vilao v4("Justin Hammer","Marvel", "roubar a tecnologia Stark e vender robos ao governo");
    Heroi h4("Homem de Ferro","Marvel");
    
    //Definido inimizades.
    h1.setInimigo(&v1);
    h2.setInimigo(&v2);
    h3.setInimigo(&v3);
    h4.setInimigo(&v4);

    //Intruduzindo heróis e vilões
    h1.introducao();
    h1.getInimigo().introducao();
    std::cout << std::endl;

    h2.introducao();
    h2.getInimigo().introducao();
    std::cout << std::endl;
    
    h3.introducao();
    h3.getInimigo().introducao();
    std::cout << std::endl;
    
    h4.introducao();
    h4.getInimigo().introducao();

    return 0;
}

