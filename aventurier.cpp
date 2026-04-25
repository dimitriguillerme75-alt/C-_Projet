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

void Aventurier::afficherStatut(int distanceSortie) const {
    cout << "\n+---------------------------------+" << endl;
    cout << " Position   : (" << x << ", " << y << ")" << endl;
    cout << " Sante      : " << pv << "/100" << endl;
    cout << " Inventaire : " << tresors << " tresor(s)" << endl;
    
    // Affichage de la distance calculée par le BFS
    if (distanceSortie != -1) {
        cout << " Distance sortie : " << distanceSortie << " cases" << endl;
    } else {
        cout << " Distance sortie : Inconnue" << endl;
    }
    cout << "+---------------------------------+\n" << endl;
}

void Aventurier::resoudreCase(Case* c) {
    // 1. Est-ce un Trésor ?
    if (dynamic_cast<Tresor*>(c) != nullptr) {
        tresors++;
        cout << "\n>>> Super ! Vous avez trouve un tresor ! <<<" << endl;
    } 
    // 2. Est-ce un Piège ?
    else if (dynamic_cast<Piege*>(c) != nullptr) {
        pv -= 10; 
        cout << "\n>>> AIE ! Vous avez marche sur un piege ! (-10 PV) <<<" << endl;
    }
    // 3. Est-ce un Monstre ?
    else if (dynamic_cast<Monstre*>(c) != nullptr) {
        cout << "\n>>> UN MONSTRE SE DRESSE DEVANT VOUS ! <<<" << endl;
        cout << "Voulez-vous (c)ombattre pour des tresors ou (f)uir ? : ";
        char choix;
        cin >> choix;

        if (choix == 'c' || choix == 'C') {
            combatMonstre(); // On lance le mini-jeu
        } else {
            cout << "\nVous fuyez comme un poltron... Le monstre ricane." << endl;
            // Note : Le monstre reste sur la case car on n'a pas combattu
        }
    }
    // 4. Est-ce la Sortie ?
    else if (dynamic_cast<Sortie*>(c) != nullptr) {
        cout << "\n>>> VOUS AVEZ TROUVE LA SORTIE ! <<<" << endl;
    }
}

void Aventurier::boucleDeJeu(Donjon& d) {
    char action;
    bool aGagne = false;

    // La boucle tourne tant qu'on est en vie et qu'on n'a pas gagné
    while (estVivant() && !aGagne) {
        // 1. Afficher l'état du jeu
        // On passe notre position x et y au donjon pour qu'il dessine le @
        d.afficher(x, y);
        // Calcul du radar BFS avant l'affichage
        int dist = d.calculerDistanceSortie(x, y);
        afficherStatut(dist);

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

        Case* caseCible = d.getCase(nx, ny);

        // 2. On vérifie si la case existe et si ce n'est pas un mur '#'
        if (caseCible != nullptr && caseCible->afficher() != '#') {
            
            // On sauvegarde le symbole AVANT de résoudre la case
            char symbole = caseCible->afficher();

            // On se déplace et on applique l'effet
            deplacer(nx, ny);
            resoudreCase(caseCible);
            
            // On vérifie les conditions de victoire ou de nettoyage
            if (symbole == 'S') {
                aGagne = true;
            } 
            // Si c'est un objet interactif, on le détruit de la carte
            else if (symbole == 'T' || symbole == 'M' || symbole == 'P') {
                d.remplacerParPassage(nx, ny);
            }

        } else {
            cout << "Aie ! Vous foncez dans un mur !" << endl;
        }
    }

    if (aGagne) {
        cout << "\n=========================================" << endl;
        cout << " FELICITATIONS ! Vous etes sorti vivant !" << endl;
        cout << " Tresors ramasses : " << tresors << endl;
        cout << "=========================================\n" << endl;
    } else if (!estVivant()) {
        cout << "\nVous etes mort... Game Over!" << endl;
    }
}

#include <random>
#include <chrono>

void Aventurier::combatMonstre() {
    int victoiresJoueur = 0;
    int victoiresMonstre = 0;
    
    // Initialisation de l'aléatoire
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine engine(seed);
    uniform_int_distribution<int> choixMonstre(1, 3); // 1: Pierre, 2: Feuille, 3: Ciseaux
    uniform_int_distribution<int> rollRecompense(1, 100);

    cout << "\n--- DEBUT DU COMBAT (Premier a 3 victoires) ---" << endl;

    while (victoiresJoueur < 3 && victoiresMonstre < 3) {
        cout << "\nScore : Vous " << victoiresJoueur << " - " << victoiresMonstre << " Monstre" << endl;
        cout << "Choisissez : (1) Pierre, (2) Feuille, (3) Ciseaux : ";
        int choixJ;
        cin >> choixJ;

        if (choixJ < 1 || choixJ > 3) {
            cout << "Choix invalide, vous perdez la manche par maladresse !" << endl;
            victoiresMonstre++;
            continue;
        }

        int choixM = choixMonstre(engine);
        string noms[] = {"", "Pierre", "Feuille", "Ciseaux"};
        cout << "Vous : " << noms[choixJ] << " | Monstre : " << noms[choixM] << endl;

        if (choixJ == choixM) {
            cout << "Egalite ! On recommence la manche." << endl;
        } else if ((choixJ == 1 && choixM == 3) || (choixJ == 2 && choixM == 1) || (choixJ == 3 && choixM == 2)) {
            cout << "Gagne ! Vous remportez la manche." << endl;
            victoiresJoueur++;
        } else {
            cout << "Perdu ! Le monstre remporte la manche." << endl;
            victoiresMonstre++;
        }
    }

    // --- RESULTATS DU MATCH ---
    if (victoiresJoueur == 3) {
        cout << "\nVICTOIRE !" << endl;
        if (victoiresMonstre == 0) {
            // Victoire 3-0 : Tirage des trésors
            int chance = rollRecompense(engine);
            int gain = 0;
            if (chance <= 10) gain = 3;      // 10% de chance
            else if (chance <= 35) gain = 2; // 25% de chance (10 + 25)
            else gain = 1;                  // 65% de chance

            tresors += gain;
            cout << "PERFECT ! Le monstre lache ses objets. Vous gagnez " << gain << " tresor(s) !" << endl;
        } else {
            cout << "Vous passez de justesse. Le monstre s'enfuit sans rien laisser." << endl;
        }
    } else {
        // Defaite
        cout << "\nDEFAITE..." << endl;
        if (tresors > 0) {
            tresors--;
            cout << "Le monstre vous a blesse et vole un tresor !" << endl;
        } else {
            pv = 0;
            cout << "Vous n'avez plus de tresors pour negocier votre vie... LE MONSTRE VOUS ACHEVE !" << endl;
        }
    }
}