#include <iostream>
#include <vector>
#include "Weapon.h"
#include "WeaponTypes.h"

using namespace std;

// Das Inventar-System (Später wird ImGui genau diese Daten auslesen!)
class Inventory {
private:
    int columns; // Raster-Spalten
    int rows;    // Raster-Zeilen
    vector<Item*> items; // Liste von Zeigern auf unsere Items

public:
    Inventory(int cols, int rows) : columns(cols), rows(rows) {}
    
    void addItem(Item* newItem) {
        items.push_back(newItem);
        cout << ">>> " << newItem->name << " in den Aktenkoffer gelegt.\n";
    }
    
    void showInventory() {
        cout << "\n=== RE INVENTAR (" << columns << "x" << rows << " Slots) ===\n";
        for (Item* item : items) {
            item->inspect(); // Polymorphie: Ruft automatisch die richtige inspect() auf!
        }
        cout << "====================================\n\n";
    }
};

int main() {
    // 1. Inventar erstellen (z.B. 8x10 Kästchen groß)
    Inventory briefcase(8, 10);
    
    // 2. Items aus deinen Konzepten erstellen (Breite, Höhe, [Schaden], [Kapazität])
    Weapon* pistol = new Weapon("Alligator Snapper (P320)", 2, 2, 150, 15);
    Weapon* shotgun = new Weapon("Serbu Super Shorty", 4, 2, 600, 3);
    Ammo* pistolAmmo = new Ammo("Pistolenmunition", 1, 1, "9mm", 30);
    Item* herb = new Item("Grünes Kraut", 1, 1);
    
    // 3. Dem Inventar hinzufügen
    briefcase.addItem(pistol);
    briefcase.addItem(shotgun);
    briefcase.addItem(pistolAmmo);
    briefcase.addItem(herb);
    
    // 4. Inventar anzeigen
    briefcase.showInventory();
    
    return 0;
}