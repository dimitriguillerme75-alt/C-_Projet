#include "aventurier.hpp"
#include "donjon.hpp"
#include "case.hpp"
#include <iostream>

using namespace std;

// Constructeur : On commence avec 100 PV et 0 trésor
Aventurier::Aventurier(int startX, int startY) : x(startX), y(startY), pv(100), tresors(0) {}

void Aventurier::deplacer(int nx, int ny) {
    x = nx;
    y = ny;
}

bool Aventurier::estVivant() const {
    return pv > 0;
}

void Aventurier::afficherStatut() const {
    cout << "\n+---------------------------------+" << endl;
    cout << " Position   : (" << x << ", " << y << ")" << endl;
    cout << " Sante      : " << pv << "/100" << endl;
    cout << " Inventaire : " << tresors << " tresor(s)" << endl;
    cout << "+---------------------------------+\n" << endl;
}

void Aventurier::resoudreCase(Case* c) {
    // Plus tard, on utilisera 'dynamic_cast' ici pour savoir si 'c' 
    // est un Tresor, un Monstre ou un Piege, et appliquer les effets.
    // Pour l'instant, c'est une coquille vide !
}

void Aventurier::boucleDeJeu(Donjon& d) {
    char action;
    bool aGagne = false;

    // La boucle tourne tant qu'on est en vie et qu'on n'a pas gagné
    while (estVivant() && !aGagne) {
        // 1. Afficher l'état du jeu
        // On passe notre position x et y au donjon pour qu'il dessine le @
        d.afficher(x, y);
        afficherStatut();

        // 2. Demander l'action au joueur
        cout << "Deplacement (z:Haut, s:Bas, q:Gauche, d:Droite) ou p:Quitter : ";
        cin >> action;

        int nx = x, ny = y;

        // Calcul de la case cible selon la touche pressée
        switch (action) {
            case 'z': ny--; break; // Haut
            case 's': ny++; break; // Bas
            case 'q': nx--; break; // Gauche
            case 'd': nx++; break; // Droite
            case 'p': cout << "Abandon..." << endl; return;
            default: cout << "Touche non reconnue." << endl; continue;
        }

        // --- LOGIQUE DE DÉPLACEMENT ET COLLISION ---

        // 1. On récupère la case sur laquelle l'aventurier veut aller
        Case* caseCible = d.getCase(nx, ny);

        // 2. On vérifie si la case existe et si ce n'est pas un mur '#'
        if (caseCible != nullptr && caseCible->afficher() != '#') {
            // On met à jour la position
            deplacer(nx, ny);
            
            // On applique l'effet de la case (ramasser trésor, combat, etc.)
            resoudreCase(caseCible);
            
            cout << "Deplacement reussi !" << endl;
        } else {
            // C'est un mur ou le bord du monde
            cout << "Aie ! Vous foncez dans un mur !" << endl;
        }
    }

    if (!estVivant()) {
        cout << "Vous etes mort... Game Over!" << endl;
    }
}