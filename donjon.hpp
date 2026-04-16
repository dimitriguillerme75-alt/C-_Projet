// Dans Donjon.hpp

#ifndef DONJON_HPP
#define DONJON_HPP

#include <vector>
#include "Case.hpp"

class Donjon {
private:
    std::vector<std::vector<Case*>> grille;
    int largeur;
    int hauteur;

    // --- NOUVEAU : Méthodes privées pour l'algorithme ---
    
    // Fonction récursive principale (x = colonne, y = ligne)
    void genererLabyrinthe(int x, int y, std::vector<std::vector<bool>>& visite);
    
    // Vérifie si des coordonnées sont valides dans la grille
    bool estValide(int x, int y);

public:
    Donjon();
    ~Donjon();

    // On garde la même signature publique
    void generer(int l, int h);
    void afficher();
};

#endif