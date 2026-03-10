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
│   ├── imgui/      # Geklonte GUI-Bibliothek Dear ImGui
│   ├── glad/       # OpenGL Loader (Header & glad.c)
│   └── stb/        # stb_image.h für Texturen
├── img/            # Enthält die Spiel-Assets (PNG-Bilder mit transparentem Hintergrund)
├── .gitignore      # Verhindert, dass kompilierte Binärdateien auf GitHub landen
└── README.md       # Diese Datei
```

---

## 🛠️ Verwendete Technologien

* **C++ (OOP):** Objektorientierte Architektur für das Backend und die Spiellogik.
* **GLFW & OpenGL3:** Übernehmen das Hardware-Rendering und das Erstellen des Fensters (Linux/X11).
* **Dear ImGui:** Zeichnet die reaktive Benutzeroberfläche und das Grid-Inventar (Frontend).
* **GLAD:** Lädt zur Laufzeit die systemabhängigen OpenGL-Funktionen.
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

## 📦 Abhängigkeiten & Installation (Linux/Debian)

Bevor das Projekt kompiliert werden kann, müssen die nötigen Bibliotheken und Systempakete installiert bzw. eingebunden werden.

### 1. Systempakete installieren (GLFW & OpenGL)
OpenGL ist hardwareseitig vorhanden, benötigt unter Linux aber die Entwickler-Header (Mesa). GLFW wird für das Fenster-Management genutzt.
```bash
sudo apt update
sudo apt install build-essential libglfw3-dev libgl1-mesa-dev
```

### 2. Dear ImGui klonen
ImGui wird direkt als Source-Code in das Projekt kompiliert.
1. Klone das Repository: `git clone https://github.com/ocornut/imgui.git`
2. Kopiere die Kern-Dateien (`imgui*.cpp`, `imgui*.h`) sowie die genutzten Backends (`backends/imgui_impl_glfw.*` und `backends/imgui_impl_opengl3.*`) in den Ordner `src/imgui/`.

### 3. GLAD (OpenGL Loader)
1. Generiere die Dateien über den [GLAD Web Service](https://glad.dav1d.de/).
2. Lade das `.zip` herunter und füge `glad.c` sowie die Header-Ordner (`KHR` und `glad`) in `src/glad/` ein.

### 4. stb_image
Lade die [stb_image.h](https://github.com/nothings/stb/blob/master/stb_image.h) herunter und platziere sie in `src/stb/`.

---

## 🚀 Kompilieren

Navigiere in das Hauptverzeichnis des Projekts und führe folgenden Befehl aus. *(Achte darauf, dass auch `glad.c` kompiliert wird, falls du es nutzt)*:

```bash
g++ src/main.cpp src/Weapon.cpp src/WeaponTypes.cpp src/glad/glad.c src/imgui/*.cpp -o game -lglfw -lGL -ldl
```
*(Hinweis: `-ldl` ist unter Linux oft nötig, damit GLAD dynamische Bibliotheken laden kann).*

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
