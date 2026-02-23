# Resident Evil 9: Requiem - Waffensystem (C++ OOP Übung)

Dieses Projekt demonstriert die grundlegenden Konzepte der objektorientierten Programmierung (OOP) in C++ anhand des spekulierten Waffenarsenals aus *Resident Evil 9: Requiem*.

## 📂 Dateistruktur
* `Weapon.h` / `Weapon.cpp`: Enthält die abstrakte Basisklasse `Weapon`. Verwaltet gemeinsame Attribute wie Name, Schaden und Munitionskapazität.
* `WeaponTypes.h` / `WeaponTypes.cpp`: Enthält die abgeleiteten Klassen:
  * `Handgun`: Standard-Pistolen (z.B. Beretta M1934).
  * `Magnum`: Hochkalibrige Waffen mit massivem Schaden (z.B. RSh-12).
  * `SMG`: Maschinenpistolen mit Salvenfeuer-Mechanik (z.B. MPA30DMG).
  * `Shotgun`: Schrotflinten mit Streuschaden (z.B. Serbu Super Shorty).
* `main.cpp`: Das Hauptprogramm zur Initialisierung des Inventars und zur Kampf-Simulation.

## 🛠️ Technische Konzepte
1. **Vererbung:** Die spezifischen Waffentypen erben Attribute (`damage`, `currentAmmo`) und Methoden (`reload()`) von der Basisklasse `Weapon`.
2. **Abstrakte Klassen:** `Weapon` ist abstrakt (besitzt die rein virtuelle Funktion `virtual void fire() = 0;`). Man kann keine direkte Instanz von "Waffe" erstellen, sondern nur von spezifischen Typen.
3. **Polymorphie:** Im Inventar (Vector in `main.cpp`) werden Zeiger vom Typ `Weapon*` gespeichert. Ruft man `fire()` auf, entscheidet das Programm zur Laufzeit automatisch, ob die Pistolen-, Shotgun- oder Magnum-Variante des Schusses ausgeführt wird.

## 🚀 Kompilieren und Ausführen
Um das Projekt im Terminal (z.B. mit GCC/G++) zu kompilieren, navigiere in den Ordner mit den Dateien und führe folgenden Befehl aus:

```bash
g++ main.cpp Weapon.cpp WeaponTypes.cpp -o re9_weapons
./re9_weapons