# Projekt: Lime Requiem (Resident Evil Inventory System)
## Projektstruktur

## Das Projekt wurde professionell aufgeräumt und strukturiert:

    src/ - Enthält den gesamten C++ Quellcode und Header-Dateien.

    src/imgui/ - Enthält die GUI-Bibliothek Dear ImGui.

    img/ - Enthält die Spiel-Assets (PNG-Bilder mit transparentem Hintergrund).

    .gitignore - Verhindert, dass kompilierte Binärdateien auf GitHub landen.

## Verwendete Technologien

    C++ (OOP): Objektorientierte Architektur für das Backend.

    GLFW & OpenGL3: Übernehmen das Erstellen des Fensters (Debian/X11).

    Dear ImGui: Zeichnet die Benutzeroberfläche und das Grid-Inventar.

    stb_image: Lädt PNG-Dateien von der Festplatte.

## C++ Klassenarchitektur (Backend)

## Die Logik basiert auf Vererbung und Polymorphismus:

    Item (Basisklasse): Beinhaltet Name, Breite, Höhe und die Position im Koffer (gridX, gridY).

    Weapon (erbt von Item): Fügt Schaden und Munition hinzu.

    Ammo (erbt von Item): Fügt Munitionstyp und Menge hinzu.

    Inventory (Der Koffer): Verwaltet ein 2D-Raster. Eine Logik prüft, ob genug freie Kästchen für ein Item vorhanden sind.

## Visuelles Rendering (Frontend)

## Das unsichtbare 2D-Raster wird durch ImGui visuell auf den Bildschirm gebracht:

    Schachbrett-Hintergrund: Eine for-Schleife zeichnet das leere Raster.

    Pixelgenaue Platzierung: Mit ImGui::SetCursorPos werden die Bilder exakt auf ihre Koordinaten gezeichnet.

    Texturen: Die PNGs werden als ImGui::ImageButton skaliert.

## Kompilieren unter Linux/Debian


`g++ src/main.cpp src/Weapon.cpp src/WeaponTypes.cpp src/imgui/*.cpp -o game -lglfw -lGL`

## Nächste geplante Schritte

    - Echte, fotorealistische transparente PNGs für die Waffen suchen und in img/ ablegen.

    - Drag-and-Drop Logik programmieren.