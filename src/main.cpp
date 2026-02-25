#include <iostream>
#include <vector>
#include "Weapon.h"
#include "WeaponTypes.h"

int main() {
    std::cout << "=== RESIDENT EVIL 9: REQUIEM - WAFFENSYSTEM INIT ===\n\n";

    // Instanziierung der bestätigten/vermuteten Waffen
    Handgun beretta("Beretta M1934", 15, 7);
    Handgun sig232("SIG Sauer P232", 18, 8);
    Handgun sig320("SIG Sauer P320 AXG (Sentinel Ten)", 22, 15);
    Magnum rsh12("RSh-12 (Requiem)", 150, 5);
    SMG mpa30("MasterPiece Arms MPA30DMG", 12, 30, 3); // 3 Schuss pro Salve
    Shotgun serbu("Serbu Super Shorty (MSBG 500)", 25, 3, 8); // 8 Schrotkugeln

    // Ein Inventar erstellen (Polymorphie im Einsatz!)
    // Wir speichern Zeiger der Basisklasse, aber sie rufen die spezifischen fire()-Methoden auf.
    std::vector<Weapon*> inventory;
    inventory.push_back(&beretta);
    inventory.push_back(&sig320);
    inventory.push_back(&rsh12);
    inventory.push_back(&mpa30);
    inventory.push_back(&serbu);

    std::cout << "--- Waffenstatus ---" << std::endl;
    for(Weapon* w : inventory) {
        w->displayStatus();
    }
    std::cout << "\n--- Kampf-Simulation ---\n" << std::endl;

    // Testfeuer
    beretta.fire();
    sig320.fire();
    rsh12.fire(); // Magnum macht mächtig Schaden
    mpa30.fire(); // SMG feuert eine Salve
    serbu.fire(); // Shotgun streut

    std::cout << "\n--- Nachladen ---\n" << std::endl;
    rsh12.reload();
    rsh12.displayStatus();

    std::cout << "\n====================================================\n";
    return 0;
}