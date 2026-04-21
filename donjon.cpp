#include "donjon.hpp"
#include <iostream>  // Pour l'affichage en console (cout, endl)
#include <algorithm> // Pour la fonction std::shuffle (qui va mélanger les directions)
#include <random>    // Pour le moteur de génération de nombres aléatoires
#include <chrono>    // Pour utiliser l'horloge du PC comme "graine" de l'aléatoire

using namespace std; // Évite de devoir écrire std:: devant vector, cout, etc.


// Constructeur : Initialise un donjon vide avec des dimensions à 0 au départ
Donjon::Donjon() : largeur(0), hauteur(0) {}

// Destructeur 
// Comme on utilise "new" pour créer nos cases dynamiquement, on doit absolument 
// utiliser "delete" quand le donjon est détruit pour libérer la mémoire 
Donjon::~Donjon() {
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            delete grille[i][j]; // Détruit chaque objet Case* un par un
        }
    }
}


// Prépare la grille et lance l'algorithme de création du labyrinthe
void Donjon::generer(int l, int h) {
    largeur = l;
    hauteur = h;
    
    // On redimensionne le tableau 2D pour qu'il ait la bonne taille (hauteur x largeur)
    // On le remplit initialement avec des pointeurs vides (nullptr)
    grille.resize(hauteur, vector<Case*>(largeur, nullptr));
    
    // Initialisation : On remplit TOUTE la grille avec des murs.
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
        }
    }

    //  On crée un tableau 2D de booléens de la même taille que la grille.
    // Il sert à mémoriser quelles cases ont déjà été creusées/visitées par l'algorithme.
    // On l'initialise entièrement à "false" (non visité).
    vector<vector<bool>> visite(hauteur, vector<bool>(largeur, false));

    //  On lance la (l'algorithme récursif) en partant de la case (1, 1).
    // On ne part pas de (0,0) pour être sûr de garder un mur d'enceinte tout autour.
    genererLabyrinthe(1, 1, visite);
    
    // Finitions : On force la création d'une Entrée et d'une Sortie.
    // On détruit le mur en haut à gauche pour l'entrée...
    delete grille[1][1];
    grille[1][1] = CaseFactory::creerCase(TypeCase::ENTREE);
    
    // ...et on détruit le mur en bas à droite pour la sortie.
    delete grille[hauteur-2][largeur-2];
    grille[hauteur-2][largeur-2] = CaseFactory::creerCase(TypeCase::SORTIE);
}

// ========================================================
// ALGORITHME DE PARCOURS (RECURSIVE BACKTRACKING)
// ========================================================

// Vérifie si la case qu'on veut creuser est bien à l'intérieur du donjon.
// On garde une marge de 1 (x > 0 et non x >= 0) pour ne pas casser les murs des bords.
bool Donjon::estValide(int x, int y) {
    return (x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1);
}

// Le cœur du moteur : l'algorithme de creusage qui s'appelle lui-même (fonction récursive)
// Il reçoit la position courante (x, y) et le tableau des cases visitées en référence (&).
void Donjon::genererLabyrinthe(int x, int y, vector<vector<bool>>& visite) {
    
    // Étape A : On marque la case sur laquelle on se trouve comme "visitée"
    visite[y][x] = true;
    
    // Étape B : On détruit le mur à cet emplacement et on le remplace par un passage
    delete grille[y][x];
    grille[y][x] = CaseFactory::creerCase(TypeCase::PASSAGE);

    // Étape C : On prépare nos 4 directions de déplacement : Est, Sud, Ouest, Nord.
    // ATTENTION : On avance de DEUX cases (ex: {2, 0}) et non d'une seule ! 
    // Pourquoi ? Pour toujours laisser un mur d'épaisseur entre deux couloirs.
    vector<pair<int, int>> directions = { {2, 0}, {0, 2}, {-2, 0}, {0, -2} };

    // Étape D : On mélange ces 4 directions aléatoirement pour que le labyrinthe 
    // soit unique à chaque exécution du programme.
    unsigned seed = chrono::system_clock::now().time_since_epoch().count(); // Graine liée à l'heure
    default_random_engine engine(seed); // Démarre le moteur aléatoire
    shuffle(directions.begin(), directions.end(), engine); // Mélange le tableau

    // Étape E : On essaie d'avancer dans chacune des 4 directions, l'une après l'autre
    for (const auto& dir : directions) {
        int nx = x + dir.first;  // Nouvelle position X (colonne cible, à +2 ou -2)
        int ny = y + dir.second; // Nouvelle position Y (ligne cible, à +2 ou -2)

        // Si la case cible (à 2 pas de distance) est dans les limites ET n'a jamais été visitée...
        if (estValide(nx, ny) && !visite[ny][nx]) {
            
            // ... on calcule les coordonnées du mur qui se trouve EXACTEMENT ENTRE 
            // notre position actuelle et la case cible (donc à 1 pas, d'où la division par 2).
            int mur_x = x + dir.first / 2;
            int mur_y = y + dir.second / 2;
            
            // On détruit ce mur intermédiaire pour relier notre case actuelle à la case cible
            delete grille[mur_y][mur_x];
            grille[mur_y][mur_x] = CaseFactory::creerCase(TypeCase::PASSAGE);

            // MAGIE DE LA RÉCURSIVITÉ :
            // On relance la fonction DEPUIS la nouvelle case cible !
            // L'algorithme va continuer de creuser jusqu'à être coincé dans une impasse.
            // Une fois coincé, la fonction se termine et l'algorithme "remonte" (backtrack) 
            // pour essayer les autres directions laissées en attente dans la boucle for.
            genererLabyrinthe(nx, ny, visite);
        }
    }
}

Case* Donjon::getCase(int x, int y) const {
    // Vérification de sécurité pour ne pas sortir du tableau
    if (y >= 0 && y < hauteur && x >= 0 && x < largeur) {
        return grille[y][x];
    }
    return nullptr;
}

void Donjon::afficher(int playerX, int playerY) {
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            // Si la case actuelle correspond à la position du joueur, on affiche @
            if (j == playerX && i == playerY) {
                cout << "@ ";
            } else {
                // Sinon, on affiche le contenu normal de la case (Mur, Passage, etc.)
                cout << grille[i][j]->afficher() << " "; 
            }
        }
        cout << endl;
    }
}