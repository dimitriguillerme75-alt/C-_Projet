#include "Donjon.hpp"
#include <iostream>

using namespace std; 

int main() {
    Donjon monDonjon;
    
    int largeur = 21;
    int hauteur = 21;

    cout << "=== Generation du donjon (" << largeur << "x" << hauteur << ") ===" << endl;
    
    monDonjon.generer(largeur, hauteur);
    monDonjon.afficher();
    
    return 0;
}