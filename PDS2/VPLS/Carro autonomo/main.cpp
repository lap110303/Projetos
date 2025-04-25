#include <iostream>
#include <iomanip>

#include "Pessoa.hpp"
#include "Carro.hpp"

using namespace std;

int main() {
  
    // Criando o Carro de acordo com a entrada
    double x, y, th;
    cin >> x >> y >> th;
    Carro carro(x, y, th);

    // Numero de pessoas na cena
    int num_pessoas;
    cin >> num_pessoas;

    // Criando um vetor de ponteiros para guardar as pessoas
    Pessoa *pessoas[num_pessoas]; 

    // Criando as pessoas da cena com os dados de entrada
    for (int i = 0; i < num_pessoas; i++) {
        cin >> x >> y;
        pessoas[i] = new Pessoa(x, y);
    }

    // Definindo o formato da saida
    cout << fixed << setprecision(2); 
    
    int idx;
    double w, v, t;

    char data;
    while (cin >> data) {   // Lendo o caractere que define o comando

        // Executando o comando informado
        switch (data) {
            
            // Imprimindo o status de todas as pessoas da cena
            case 's': {                
                for (int i = 0; i < num_pessoas; i++) {
                    std::cout << pessoas[i]->getX() << "\t" << pessoas[i]->getY() << std::endl;
                }
                break;
            }
            
            // Mover pessoa
            case 'm': {                 
                cin >> idx; // Indice da pessoa que vai mover

                double dx, dy;
                cin >> dx >> dy;

                pessoas[idx]->mover(dx, dy);
                
                break;                              
            }

            // Imprimindo o status do carro
            case 'c': {                
                std::cout << carro.getX() << "\t" << carro.getY() << "\t" << carro.getTH() << std::endl;
                break;
            }    

            // Girando o carro
            case 'g': {     
                cin >> w >> t;          
                carro.girar(w, t);
                break;
            }     

            // Mover o carro em frente
            case 'f': {     
                cin >> v >> t;          
                carro.mover(v, t);
                break;
            }    

            // Calculo o ângulo para uma certa pessoa
            case 'a': {     
                cin >> idx;          
                double a = carro.calcular_angulo_pessoa(pessoas[idx]);
                std::cout << a << std:: endl;
                break;
            }  

            // Calculo da distância para uma certa pessoa
            case 'd': {     
                cin >> idx;          
                double d = carro.calcular_distancia_pessoa(pessoas[idx]);
                std::cout << d << std:: endl; 
                break;
            }                                            

            // Verifica movimentação permitida
            case 'p': {                 
                bool p = carro.movimentacao_permitida(pessoas, num_pessoas);
                std::cout << p << std::endl;
                break;                              
            }

            // Realizando navegacão completa
            case 'n': {                 
                cin >> w >> v >> t; 
                carro.navegar(pessoas, num_pessoas, w, v, t);
                break;                              
            }

        }
    }
    
    // Desalocando os ponteiros do vetor
    for (int i = 0; i < num_pessoas; i++) {
        delete pessoas[i];
    }

    return 0;
}