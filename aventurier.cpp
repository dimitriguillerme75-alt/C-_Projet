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
    // case trésor
    if (dynamic_cast<Tresor*>(c) != nullptr) {
        tresors++;
        cout << "\n>>> Vous avez trouvé un tresor ! (+1 trésor) <<<" << endl;
    } 
    // case piège
    else if (dynamic_cast<Piege*>(c) != nullptr) {
        pv -= 20; 
        cout << "\n>>> Vous avez marché sur un piège ! (-20 PV) <<<" << endl;
    }
    // case monstre
    else if (dynamic_cast<Monstre*>(c) != nullptr) {
        cout << "\n>>> Un monstre vous attaque ! <<<" << endl;
        cout << "Voulez-vous (c)ombattre pour des trésors ou (f)uir ? (c/f): ";
        char choix;
        cin >> choix;

        // si il choisit la touche c on lance le combat (pierre/feuille/ciseaux)
        if (choix == 'c' || choix == 'C') {
            combatMonstre(); 
        // si il choisit la touche f (ou autre que c), 
        // on passe à la case suivante et le monstre reste sur la case
        } else {
            cout << "\nVous fuyez par peur, la honteee" << endl;
        }
    }
    // case sortie
    else if (dynamic_cast<Sortie*>(c) != nullptr) {
        cout << "\n>>> VOUS AVEZ TROUVE LA SORTIE, A VOUS LA RICHESSE !!! <<<" << endl;
    }
}

void Aventurier::boucleDeJeu(Donjon& d) {
    char action;
    bool aGagne = false;

    // La boucle tourne tant qu'on est en vie et qu'on n'a pas gagné
    while (estVivant() && !aGagne) {
        // Affiche l'état du jeu
        // On passe notre position x et y au donjon pour qu'il dessine le @
        d.afficher(x, y);
        // Calcul du radar BFS avant l'affichage
        int dist = d.calculerDistanceSortie(x, y);
        afficherStatut(dist);

        // On demande l'action au joueur
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
            default: cout << "Touche non reconnue, réessayez." << endl; continue;
        }

        // logique de déplacement et collision

        Case* caseCible = d.getCase(nx, ny);

        // On vérifie si la case existe et si ce n'est pas un mur (#)
        if (caseCible != nullptr && caseCible->afficher() != '#') {
            
            // On sauvegarde le symbole avant de résoudre la case
            char symbole = caseCible->afficher();

            // On se déplace et on applique l'effet
            deplacer(nx, ny);
            resoudreCase(caseCible);
            
            // Si on arrive sur la sortie, on gagne
            if (symbole == 'S') {
                aGagne = true;
            } 
            // Si c'est un objet interactif, on le détruit en le remplaçant par un simple passage
            else if (symbole == 'T' || symbole == 'M' || symbole == 'P') {
                d.remplacerParPassage(nx, ny);
            }

        } else {
            cout << "Vous foncez dans un mur, regardez devant vous !" << endl;
        }
    }

    if (aGagne) {
        cout << "\n=========================================" << endl;
        cout << " FELICITATIONS ! Vous êtes sorti vivant !" << endl;
        cout << " Trésors ramassés : " << tresors << endl;
        cout << "=========================================\n" << endl;
    } else if (!estVivant()) {
        cout << "\nVous êtes mort lamentablement... >>> DEFAITE ! <<< " << endl;
    }
}

#include <random>
#include <chrono>

void Aventurier::combatMonstre() {
    int victoiresJoueur = 0;
    int victoiresMonstre = 0;
    
    // On initialise l'aléatoire
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine engine(seed);
    uniform_int_distribution<int> choixMonstre(1, 3); // 1 = Pierre, 2 = Feuille, 3 = Ciseaux
    uniform_int_distribution<int> rollRecompense(1, 100);

    cout << "\n--- DEBUT DU COMBAT (Premier à 3 victoires) ---" << endl;

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
            cout << "Egalité ! On recommence la manche." << endl;
        } else if ((choixJ == 1 && choixM == 3) || (choixJ == 2 && choixM == 1) || (choixJ == 3 && choixM == 2)) {
            cout << "Gagné ! Vous remportez la manche." << endl;
            victoiresJoueur++;
        } else {
            cout << "Perdu ! Le monstre remporte la manche." << endl;
            victoiresMonstre++;
        }
    }

    // Résultat du combat 

    // Si le joueur gagne
    if (victoiresJoueur == 3) {
        cout << "\nVICTOIRE ! le Monstre s'en va désespéré..." << endl;
        // Si le joueur a gagné 3-0, il gagne un certain nombre de trésor
        if (victoiresMonstre == 0) {
            int chance = rollRecompense(engine);
            int gain = 0;
            if (chance <= 10) gain = 3;      // 10% de chance de gagner 3 trésor
            else if (chance <= 35) gain = 2; // 25% de chance de gagner 2 trésor
            else gain = 1;                  // 65% de chance de gagner 1 trésor

            tresors += gain;
            cout << "3-0! Vous ramassez le butin du monstre. Vous gagnez " << gain << " tresor(s) !" << endl;
        } 
        // Si le joueur gagne 3-1 ou 3-2
        else {
            cout << "Vous passez de justesse. Le monstre s'enfuit en gardant son butin." << endl;
        }
    } 
    // Si le joueur perd
    else {
        cout << "\nDEFAITE..." << endl;
        if (tresors > 0) {
            tresors--;
            cout << "Le monstre vous vole un trésor !" << endl;
        } else {
            pv = 0;
            cout << "Vous n'avez plus de tresors pour négocier votre vie... !" << endl;
        }
    }
}