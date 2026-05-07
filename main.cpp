#include "donjon.hpp"
#include "aventurier.hpp" 
#include <iostream>
using namespace std; 

int main() {
    Donjon monDonjon;
    int largeur = 21;
    int hauteur = 21;

    cout << "Generation du donjon de taille : (" << largeur << "x" << hauteur << ") " << endl;
    
    // On génère le donjon
    monDonjon.generer(largeur, hauteur);
    
    // On crée notre héros à la position (1, 1)
    Aventurier joueur(1, 1);

    // On lance la boucle infinie du jeu 
    joueur.boucleDeJeu(monDonjon);
    
    return 0;
}