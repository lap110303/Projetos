#include<iostream>
#include<string>

using namespace std;

int main() {
    
    string letras;
    string palavras;
    string erradas[10];
    int j = 0;
    int x = 0;
    int cont = 0;
    bool frase = true;
    bool errada = false;
    
    cin >> letras;
    cout << "Frase corrigida:";
    
    while(cin >> palavras){
        for(unsigned int i = 0; i < palavras.size(); i ++){
            if (palavras[i] == '*'){
                palavras[i] = letras[j];
                j++;
                errada = true;
            }
        }
        cout << " " << palavras;
        if (frase){
            //cout << endl << "Total de " << cont << " palavra(s) com problemas:";
            frase = false;
        }
        if (errada){
            erradas[x] = palavras;
            x++;
            cont++;
            errada = false;
        }
    }
    
    cout << endl << "Total de " << cont << " palavra(s) com problema:";
    for(x = 0; x < 10; x++){
        cout << endl << erradas[x];
    }
    
    return 0;
}