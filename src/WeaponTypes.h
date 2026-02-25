#ifndef WEAPONTYPES_H
#define WEAPONTYPES_H

#include "Weapon.h" // Wir brauchen die Item-Basisklasse von hier
#include <string>

// --- MUNITIONS KLASSE ---
class Ammo : public Item {
public:
    std::string ammoType;
    int amount; // Wie viele Patronen sind in der Schachtel?

    Ammo(std::string n, int w, int h, std::string type, int amt);
    
    void inspect() const override;
};

#endif