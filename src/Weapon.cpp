#include "Weapon.h"

// --- IMPLEMENTIERUNG ITEM ---
Item::Item(std::string n, int w, int h) : name(n), gridWidth(w), gridHeight(h) {}

void Item::inspect() const {
    std::cout << "- " << name << " (Belegt " << gridWidth << "x" << gridHeight << " Slots)\n";
}

// --- IMPLEMENTIERUNG WEAPON ---
// Der Konstruktor ruft direkt den Konstruktor der Basisklasse (Item) auf
Weapon::Weapon(std::string n, int w, int h, int dmg, int cap) 
    : Item(n, w, h), damage(dmg), ammoCapacity(cap), currentAmmo(cap) {}

void Weapon::inspect() const {
    std::cout << "- [WAFFE] " << name << " | Dmg: " << damage 
              << " | Munition: " << currentAmmo << "/" << ammoCapacity 
              << " (" << gridWidth << "x" << gridHeight << " Slots)\n";
}