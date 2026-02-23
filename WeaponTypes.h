#pragma once
#include "Weapon.h"

// --- Klasse für Handfeuerwaffen ---
class Handgun : public Weapon {
public:
    Handgun(std::string name, int damage, int capacity);
    void fire() override; // Überschreibt die fire-Methode der Basisklasse
};

// --- Klasse für Magnum/Revolver ---
class Magnum : public Weapon {
public:
    Magnum(std::string name, int damage, int capacity);
    void fire() override;
};

// --- Klasse für Maschinenpistolen (SMG) ---
class SMG : public Weapon {
private:
    int fireRate; // Spezifische Eigenschaft nur für SMGs
public:
    SMG(std::string name, int damage, int capacity, int rate);
    void fire() override;
};

// --- Klasse für Schrotflinten ---
class Shotgun : public Weapon {
private:
    int pelletCount; // Anzahl der Schrotkugeln pro Schuss
public:
    Shotgun(std::string name, int damage, int capacity, int pellets);
    void fire() override;
};