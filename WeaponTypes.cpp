#include "WeaponTypes.h"

// --- Handgun Implementierung ---
Handgun::Handgun(std::string name, int damage, int capacity) 
    : Weapon(name, damage, capacity) {}

void Handgun::fire() {
    if (currentAmmo > 0) {
        currentAmmo--;
        std::cout << "PENG! " << name << " feuert. (-" << damage << " HP) | Munition übrig: " << currentAmmo << "\n";
    } else {
        std::cout << "*Klick* " << name << " ist leer!\n";
    }
}

// --- Magnum Implementierung ---
Magnum::Magnum(std::string name, int damage, int capacity) 
    : Weapon(name, damage, capacity) {}

void Magnum::fire() {
    if (currentAmmo > 0) {
        currentAmmo--;
        std::cout << "KABUMM! " << name << " durchschlägt das Ziel! Rückstoß ist enorm. (-" << damage << " HP) | Munition: " << currentAmmo << "\n";
    } else {
        std::cout << "*Klick* " << name << " ist leer!\n";
    }
}

// --- SMG Implementierung ---
SMG::SMG(std::string name, int damage, int capacity, int rate) 
    : Weapon(name, damage, capacity), fireRate(rate) {}

void SMG::fire() {
    if (currentAmmo >= fireRate) {
        currentAmmo -= fireRate;
        std::cout << "RATATATA! " << name << " feuert eine Salve von " << fireRate << " Schuss. (-" << (damage * fireRate) << " HP) | Munition: " << currentAmmo << "\n";
    } else if (currentAmmo > 0) {
        std::cout << "RATA! " << name << " feuert den Rest (" << currentAmmo << " Schuss). (-" << (damage * currentAmmo) << " HP)\n";
        currentAmmo = 0;
    } else {
        std::cout << "*Klick* " << name << " ist leer!\n";
    }
}

// --- Shotgun Implementierung ---
Shotgun::Shotgun(std::string name, int damage, int capacity, int pellets) 
    : Weapon(name, damage, capacity), pelletCount(pellets) {}

void Shotgun::fire() {
    if (currentAmmo > 0) {
        currentAmmo--;
        std::cout << "BÄM! " << name << " streut " << pelletCount << " Kugeln. (-" << (damage * pelletCount) << " HP auf Nahdistanz) | Munition: " << currentAmmo << "\n";
    } else {
        std::cout << "*Klick* " << name << " ist leer!\n";
    }
}