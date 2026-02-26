# 🎒 Lime Requiem: Resident Evil Inventory System

![Version](https://img.shields.io/badge/version-1.0.4-blue)
![C++](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)
![OpenGL](https://img.shields.io/badge/OpenGL-3-5586A4?logo=opengl)

Ein interaktives, Grid-basiertes Inventarsystem, inspiriert vom klassischen *Resident Evil 4* Aktenkoffer. Entwickelt in C++ mit OpenGL und Dear ImGui.

<p align="center">
  <img src="/img/gui_1.png" width="800" alt="GUI Screenshot">
</p>

---

## ✨ Features

* **Grid-basiertes System:** Waffen und Items nehmen realistische Maße (z.B. 4x2 oder 2x2 Slots) innerhalb eines 10x8 Aktenkoffers ein.
* **Drag & Drop:** Items können flüssig mit der Maus per Drag & Drop innerhalb des Koffers verschoben werden.
* **Kollisionserkennung:** Ein Item kann nur dort abgelegt werden, wo ausreichend freier Platz im Raster vorhanden ist.
* **Dynamische Ausrüstung:** Eine Seitenleiste ermöglicht das interaktive Hinzufügen (Spawnen) und Entfernen von Gegenständen in Echtzeit.
* **Echte Texturen:** Einbindung von transparenten, hochauflösenden PNG-Grafiken für fotorealistische Waffenmodelle.

---

## 📂 Projektstruktur

```text
Lime_requiem/
├── src/            # Enthält den gesamten C++ Quellcode und Header-Dateien (.cpp, .h)
│   └── imgui/      # Enthält die GUI-Bibliothek Dear ImGui
├── img/            # Enthält die Spiel-Assets (PNG-Bilder mit transparentem Hintergrund)
├── .gitignore      # Verhindert, dass kompilierte Binärdateien auf GitHub landen
└── README.md       # Diese Datei
```

---

## 🛠️ Verwendete Technologien

* **C++ (OOP):** Objektorientierte Architektur für das Backend und die Spiellogik.
* **GLFW & OpenGL3:** Übernehmen das Hardware-Rendering und das Erstellen des Fensters (Linux/X11).
* **Dear ImGui:** Zeichnet die reaktive Benutzeroberfläche und das Grid-Inventar (Frontend).
* **stb_image:** Zum Laden und Dekodieren der PNG-Dateien von der Festplatte in OpenGL-Texturen.

---

## 🧠 Architektur (Backend & Frontend)

### C++ Klassenarchitektur (Logik)
Die Logik basiert auf Vererbung und Polymorphismus:
* `Item` *(Basisklasse)*: Beinhaltet Name, Breite, Höhe und die Position im Koffer (`gridX`, `gridY`).
* `Weapon` *(erbt von Item)*: Fügt spezifische Attribute wie Schaden und Magazingröße hinzu.
* `Ammo` *(erbt von Item)*: Fügt Munitionstyp und Menge hinzu.
* `Inventory` *(Der Koffer)*: Verwaltet ein 2D-Raster. Die interne Logik validiert Bewegungen und prüft freie Rasterzellen.

### Visuelles Rendering (Frontend)
Das unsichtbare 2D-Raster wird durch ImGui visuell auf den Bildschirm gebracht:
* **Schachbrett-Hintergrund:** Eine strukturierte Schleife zeichnet das Raster als Drop-Ziele.
* **Pixelgenaue Platzierung:** Mit `ImGui::SetCursorPos` werden die Bilder exakt auf ihre logischen Koordinaten gezeichnet.
* **ImGui-Texturen:** Die PNGs werden als anklickbare und verschiebbare `ImGui::ImageButton` gerendert.

---

## 🚀 Kompilieren unter Linux/Debian

Stelle sicher, dass `g++`, `glfw3` und die OpenGL-Bibliotheken auf deinem System installiert sind. Navigiere in das Hauptverzeichnis des Projekts und führe folgenden Befehl aus:

```bash
g++ src/main.cpp src/Weapon.cpp src/WeaponTypes.cpp src/imgui/*.cpp -o game -lglfw -lGL
```

Zum Starten des Programms:
```bash
./game
```

---

## 🗺️ Nächste geplante Schritte (Roadmap)

- [x] Echte, fotorealistische transparente PNGs für die Waffen einbinden.
- [x] Drag-and-Drop Logik programmieren.
- [ ] **Item-Rotation:** Waffen per Rechtsklick drehen (z.B. von 4x2 auf 2x4 Slots), um Platz im Koffer optimal zu nutzen.
- [ ] **Koffer-Erweiterung:** Möglichkeit, den Koffer im Spielverlauf zu vergrößern.