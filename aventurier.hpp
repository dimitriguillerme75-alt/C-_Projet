// pour éviter les inclusions multiples
#ifndef AVENTURIER_HPP
#define AVENTURIER_HPP

// Déclarations anticipées 
class Donjon;
class Case;

class Aventurier {
    private:
        int x, y;       // Position du joueur sur la grille
        int pv;         // Points de vie
        int tresors;    // Nombre de trésors ramassés

    public:
        // Constructeur : initialise l'aventurier à une position de départ
        Aventurier(int startX, int startY);

        // Getters
        int getX() const { return x; }
        int getY() const { return y; }

        // Méthodes à définir
        void deplacer(int nx, int ny);
        void resoudreCase(Case* c);
        bool estVivant() const;
        void afficherStatut(int distanceSortie) const;
        void combatMonstre();
        bool fuirMonstre();
        void boucleDeJeu(Donjon& d);
};

#endif