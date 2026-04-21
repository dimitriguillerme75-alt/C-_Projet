#include "donjon.hpp"
#include "aventurier.hpp" // Ne pas oublier d'inclure notre nouveau joueur !
#include <iostream>

using namespace std; 

int main() {
    Donjon monDonjon;
    int largeur = 21;
    int hauteur = 21;

    cout << "=== Generation du donjon (" << largeur << "x" << hauteur << ") ===" << endl;
    
    // 1. On prépare le terrain
    monDonjon.generer(largeur, hauteur);
    
    // 2. On crée notre héros à la position (1, 1), qui correspond à l'Entrée 'E'
    Aventurier joueur(1, 1);

    // 3. On lance la boucle infinie du jeu !
    // (L'affichage du donjon se fera désormais à l'intérieur de cette boucle)
    joueur.boucleDeJeu(monDonjon);
    
    return 0;
}