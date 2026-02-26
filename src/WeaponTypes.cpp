#include "WeaponTypes.h"

Ammo::Ammo(std::string n, int w, int h, std::string type, int amt)
    : Item(n, w, h), ammoType(type), amount(amt) {}

void Ammo::inspect() const {
    std::cout << "- [MUNITION] " << name << " (" << amount << " Schuss " 
              << ammoType << ") (" << gridWidth << "x" << gridHeight << " Slots)\n";
}