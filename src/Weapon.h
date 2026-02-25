#ifndef WEAPON_H
#define WEAPON_H

#include <string>
#include <iostream>

// --- BASISKLASSE ---
// Alles, was im Inventar liegen kann (Waffen, Munition, Kräuter), ist ein "Item"
class Item {
public:
    std::string name;
    int gridWidth;  // Breite in Inventar-Kästchen
    int gridHeight; // Höhe in Inventar-Kästchen

    // Die Position im Koffer
    int gridX = -1; // -1 bedeutet "Noch nicht im Koffer"
    int gridY = -1;

    // Konstruktor
    Item(std::string n, int w, int h);
    
    // Virtuelle Methode: Erlaubt es Unterklassen, diese Funktion zu überschreiben (Polymorphie)
    virtual void inspect() const; 
    
    // Ein virtueller Destruktor ist wichtig, wenn man mit Vererbung arbeitet!
    virtual ~Item() = default;    
};

// --- WAFFEN KLASSE ---
// Erbt alle Eigenschaften von Item (name, gridWidth, gridHeight)
class Weapon : public Item {
public:
    int damage;
    int ammoCapacity;
    int currentAmmo;

    // Konstruktor für die Waffe
    Weapon(std::string n, int w, int h, int dmg, int cap);
    
    // Überschreibt die inspect-Methode der Basisklasse
    void inspect() const override; 
};

#endif