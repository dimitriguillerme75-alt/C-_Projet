#ifndef CASE_HPP
#define CASE_HPP

// 1. L'énumération des types possibles (demandé par le PDF)
enum class TypeCase {
    MUR,
    PASSAGE,
    ENTREE,
    SORTIE,
    TRESOR, // Prévu pour plus tard
    MONSTRE, // Prévu pour plus tard
    PIEGE   // Prévu pour plus tard
};

// --- Les classes existantes ---

class Case {
public:
    virtual char afficher() = 0;
    virtual ~Case() = default; 
};

class Mur : public Case {
public:
    char afficher() override { return '#'; }
};

class Passage : public Case {
public:
    char afficher() override { return ' '; }
};  

class Entree : public Case {
public:
    char afficher() override { return 'E'; }
};

class Sortie : public Case {
public:
    char afficher() override { return 'S'; }
};

// 2. NOUVEAU : Le design pattern Factory !
class CaseFactory {
public:
    // Méthode statique pour créer n'importe quelle case
    static Case* creerCase(TypeCase type) {
        switch (type) {
            case TypeCase::MUR:     return new Mur();
            case TypeCase::PASSAGE: return new Passage();
            case TypeCase::ENTREE:  return new Entree();
            case TypeCase::SORTIE:  return new Sortie();
            // Tu pourras décommenter la suite quand tu auras créé les classes
            // case TypeCase::TRESOR:  return new Tresor();
            // case TypeCase::MONSTRE: return new Monstre();
            // case TypeCase::PIEGE:   return new Piege();
            default: return nullptr;
        }
    }
};

#endif