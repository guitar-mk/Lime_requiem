#pragma once // Verhindert mehrfaches Einbinden der Header-Datei
#include <string>
#include <iostream>

// Abstrakte Basisklasse für alle Waffen
class Weapon {
protected: // "protected" bedeutet: Abgeleitete Klassen dürfen darauf zugreifen, von außen (main) ist es gesperrt.
    std::string name;
    int damage;
    int ammoCapacity;
    int currentAmmo;

public:
    // Konstruktor
    Weapon(std::string wName, int wDamage, int wCapacity);
    
    // Virtueller Destruktor (Wichtig bei Vererbung, damit Speicher sauber freigegeben wird!)
    virtual ~Weapon() = default;

    // Rein virtuelle Funktion ("= 0"). Macht die Klasse abstrakt.
    // Jede abgeleitete Waffe MUSS definieren, wie sie feuert.
    virtual void fire() = 0; 

    // Gemeinsame Funktionen für alle Waffen
    void reload();
    void displayStatus() const;
    std::string getName() const;
};