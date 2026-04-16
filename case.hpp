#ifndef CASE_HPP
#define CASE_HPP

// Classe abstraite de base
class Case {
public:
    virtual char afficher() = 0;
    virtual ~Case() = default; 
};

// Classe Mur
class Mur : public Case {
public:
    char afficher() override { 
        return '#'; 
    }
};
class Passage : public Case {
public:
    char afficher() override { 
        return ' '; // Un espace vide pour un passage
    }
};  

class Entree : public Case {
public:
    char afficher() override { 
        return 'E'; 
    }
};

class Sortie : public Case {
public:
    char afficher() override { 
        return 'S'; // 'S' pour Sortie
    }
};

#endif