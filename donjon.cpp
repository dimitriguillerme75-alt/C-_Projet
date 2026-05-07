#include "donjon.hpp"
#include <iostream>  
#include <algorithm> // Pour la fonction std::shuffle (qui va mélanger les directions)
#include <random>    
#include <chrono> 
#include <queue>
using namespace std; 


// Constructeur qui initialise un donjon vide avec des dimensions à 0 au départ
Donjon::Donjon() : largeur(0), hauteur(0) {}

// Destructeur (dynamique car la grille est de classe vecteur)
Donjon::~Donjon() {
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            delete grille[i][j]; 
        }
    }
}

// A BIEN COMPRENDRE ET COMMENTER
// Prépare la grille et lance l'algorithme de création du labyrinthe
void Donjon::generer(int l, int h) {
    largeur = l;
    hauteur = h;
    
    // On redimensionne le tableau 2D pour qu'il ait la bonne taille (hauteur x largeur)
    // On le remplit initialement avec des pointeurs vides (nullptr)
    grille.resize(hauteur, vector<Case*>(largeur, nullptr));
    
    // Initialisation : On remplit toute la grille avec des murs.
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            grille[i][j] = CaseFactory::creerCase(TypeCase::MUR);
        }
    }

    //  On crée un tableau 2D de booléens de la même taille que la grille.
    // Il sert à mémoriser quelles cases ont déjà été creusées/visitées par l'algorithme.
    // On l'initialise entièrement à false au début (cases non visité).
    vector<vector<bool>> visite(hauteur, vector<bool>(largeur, false));

    //  On lance la (l'algorithme récursif) en partant de la case (1, 1) 
    genererLabyrinthe(1, 1, visite);
    
    // On détruit le mur en haut à gauche pour  y mettre l'entrée
    delete grille[1][1];
    grille[1][1] = CaseFactory::creerCase(TypeCase::ENTREE);
    
    // on détruit le mur en bas à droite pour y mettre la sortie
    delete grille[hauteur-2][largeur-2];
    grille[hauteur-2][largeur-2] = CaseFactory::creerCase(TypeCase::SORTIE);
    placerElements();
}


// On vérifie si la case qu'on veut creuser est bien à l'intérieur du donjon.
// ( > 0 et < largeur - 1 pour bien laisser les murs aux extrémités)
bool Donjon::estValide(int x, int y) {
    return (x > 0 && x < largeur - 1 && y > 0 && y < hauteur - 1);
}

// BIEN COMPRENDRE SYSTEME RECURSIF
// Le cœur du moteur : l'algorithme de creusage qui s'appelle lui-même (fonction récursive)
// Il reçoit la position courante (x, y) et le tableau des cases visitées en référence (&).
void Donjon::genererLabyrinthe(int x, int y, vector<vector<bool>>& visite) {
    
    // On marque la case sur laquelle on se trouve comme visitée 
    // (en modifiant le tableau de bool initialisé dans la fonction generer)
    visite[y][x] = true;
    
    // On détruit le mur à cet emplacement et on le remplace par un passage
    delete grille[y][x];
    grille[y][x] = CaseFactory::creerCase(TypeCase::PASSAGE);

    // On prépare nos 4 directions de déplacement (Haut, Bas, Gauche et Droite)
    // On avance de deux cases Pour toujours laisser un mur d'épaisseur entre deux couloirs.
    vector<pair<int, int>> directions = { {2, 0}, {0, 2}, {-2, 0}, {0, -2} };

    // On mélange ensuite ces 4 directions aléatoirement pour que le labyrinthe 
    // soit unique à chaque exécution du programme.
    unsigned seed = chrono::system_clock::now().time_since_epoch().count(); // Graine liée à l'heure
    default_random_engine engine(seed); // Démarre le moteur aléatoire
    shuffle(directions.begin(), directions.end(), engine); // Mélange le tableau

    // On essaie d'avancer dans chacune des 4 directions, l'une après l'autre
    for (const auto& dir : directions) {
        int nx = x + dir.first;  // Nouvelle position X (colonne cible, à +2 ou -2)
        int ny = y + dir.second; // Nouvelle position Y (ligne cible, à +2 ou -2)

        // Si la case cible (à 2 pas de distance) est dans les limites et n'a jamais été visitée
        if (estValide(nx, ny) && !visite[ny][nx]) {
            
            // on calcule les coordonnées du mur qui se trouve exactement
            // notre position actuelle et la case cible (donc à 1 pas)
            int mur_x = x + dir.first / 2;
            int mur_y = y + dir.second / 2;
            
            // On détruit ce mur intermédiaire pour relier notre case actuelle à la case cible
            delete grille[mur_y][mur_x];
            grille[mur_y][mur_x] = CaseFactory::creerCase(TypeCase::PASSAGE);

            // Ici système récursif :
            // On relance la fonction depuis la nouvelle case cible 
            // L'algorithme va continuer de creuser jusqu'à être coincé dans une impasse.
            // Une fois coincé, la fonction se termine et l'algorithme "remonte" (backtrack) 
            // pour essayer les autres directions laissées en attente dans la boucle for.
            genererLabyrinthe(nx, ny, visite);
        }
    }
}

// Pour accéder à la case 
Case* Donjon::getCase(int x, int y) const {
    // Vérification de sécurité pour ne pas sortir du tableau
    if (y >= 0 && y < hauteur && x >= 0 && x < largeur) {
        return grille[y][x];
    }
    return nullptr;
}

// Pour afficher la case 
void Donjon::afficher(int playerX, int playerY) {
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            // Si la case actuelle correspond à la position du joueur, on affiche @
            if (j == playerX && i == playerY) {
                cout << "@ ";
            } 
            // Sinon on affiche le contenu normal de la case (Mur, Passage ...)
            else {
                cout << grille[i][j]->afficher() << " "; 
            }
        }
        cout << endl;
    }
}

// Pour placer les différents type de case dans le labyrinthe aléatoirement
// Modifier les probabilités ici si on veut plus ou moins de monstres, trésors ...
void Donjon::placerElements() {
    // On prépare un générateur de nombres aléatoires (entre 0 et 100)
    // qui va permettre de placer les différents types de cases aléatoirements
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine engine(seed);
    uniform_int_distribution<int> distribution(0, 100);

    // On parcourt toute la grille en évitant les murs extérieurs
    for (int i = 1; i < hauteur - 1; ++i) {
        for (int j = 1; j < largeur - 1; ++j) {
            
            // On vérifie si la case actuelle est un Passage
            // dynamic_cast teste si grille[i][j] est bien de type Passage* 
            // et non une autre classe fille de case
            if (dynamic_cast<Passage*>(grille[i][j]) != nullptr) {
                
                // On tire un nombre aléatoire
                int r = distribution(engine);
                
                // Puis on applique les probabilités (3% Trésor, 7% Monstre, 5% Piège)
                if (r < 3) {
                    // On détruit la case de type passage
                    // Pour y créer une case d'un autre type (Trésor ici)
                    delete grille[i][j]; 
                    grille[i][j] = CaseFactory::creerCase(TypeCase::TRESOR);
                } 
                else if (r < 10) { 
                    delete grille[i][j];
                    grille[i][j] = CaseFactory::creerCase(TypeCase::MONSTRE);
                } 
                else if (r < 15) {
                    delete grille[i][j];
                    grille[i][j] = CaseFactory::creerCase(TypeCase::PIEGE);
                }
                // Si r >= 15, la case reste un passage
            }
        }
    }
}

// Pour mettre un passage à la place d'une case avec un effet après qu'on y soit passé
void Donjon::remplacerParPassage(int x, int y) {
    // On vérifie que on remplace bien une case de comprise dans notre labyrinthe
    if (x >= 0 && x < largeur && y >= 0 && y < hauteur) {
        delete grille[y][x]; 
        grille[y][x] = CaseFactory::creerCase(TypeCase::PASSAGE); 
    }
}

// COMPRENDRE BIEN LE FONCTIONNEMENT (ALGO BFS? ) ET COMMENTER
int Donjon::calculerDistanceSortie(int startX, int startY) const {
    // 1. Tableau des distances (-1 signifie non visité)
    vector<vector<int>> distances(hauteur, vector<int>(largeur, -1));
    
    // 2. File pour le BFS
    queue<pair<int, int>> file;
    
    file.push({startX, startY});
    distances[startY][startX] = 0;

    vector<pair<int, int>> directions = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    while (!file.empty()) {
        pair<int, int> actuel = file.front();
        file.pop();

        int cx = actuel.first;
        int cy = actuel.second;

        // Si on a atteint la sortie, on renvoie la distance accumulée
        if (grille[cy][cx]->afficher() == 'S') {
            return distances[cy][cx];
        }

        // On explore les voisins
        for (auto dir : directions) {
            int nx = cx + dir.first;
            int ny = cy + dir.second;

            if (nx >= 0 && nx < largeur && ny >= 0 && ny < hauteur &&
                distances[ny][nx] == -1 && grille[ny][nx]->afficher() != '#') {
                
                distances[ny][nx] = distances[cy][cx] + 1;
                file.push({nx, ny});
            }
        }
    }
    return -1; // Chemin impossible
}