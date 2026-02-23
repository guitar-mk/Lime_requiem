#include "Weapon.h"

// Konstruktor-Implementierung
Weapon::Weapon(std::string wName, int wDamage, int wCapacity)
    : name(wName), damage(wDamage), ammoCapacity(wCapacity), currentAmmo(wCapacity) {
    // currentAmmo ist zu Beginn voll
}

void Weapon::reload() {
    std::cout << "-> Lade " << name << " nach..." << std::endl;
    currentAmmo = ammoCapacity;
}

void Weapon::displayStatus() const {
    std::cout << "[" << name << "] Schaden: " << damage 
              << " | Munition: " << currentAmmo << "/" << ammoCapacity << std::endl;
}

std::string Weapon::getName() const {
    return name;
}