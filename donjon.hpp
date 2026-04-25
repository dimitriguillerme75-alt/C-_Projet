// Dans Donjon.hpp

#ifndef DONJON_HPP
#define DONJON_HPP
#include <queue>
#include <vector>
#include "case.hpp"

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
    // place tresor monstre et piege
    void placerElements();


public:
    Donjon();
    ~Donjon();
    Case* getCase(int x, int y) const;
    // On garde la même signature publique
    void generer(int l, int h);
    // le -1 pour pour continuer a appeler afficher sans paramètre
    void afficher(int playerX = -1, int playerY = -1);
    // Remplace la case aux coordonnées (x, y) par un Passage
    void remplacerParPassage(int x, int y);
    // Dans la section public de la classe Donjon
    int calculerDistanceSortie(int startX, int startY) const;
};

#endif