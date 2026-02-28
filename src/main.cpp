#include <iostream>
#include <vector>
#include <string>

// stb_image.h für das Laden von PNGs in OpenGL
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ImGui und Backend-Bibliotheken
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> 

// Deine eigenen Klassen
#include "Weapon.h"
#include "WeaponTypes.h"

// --- NEU: Struktur für unsere Waffendetails ---
// Wir bündeln die Zusatzinfos hier, damit wir sie für das Info-Fenster abrufen können.
struct ItemMetadata {
    std::string category;     // "Waffen", "Munition" oder "Misc"
    std::string manufacturer; // z.B. "Beretta"
    int capacity;             // Magazingröße
};

// ItemRegistry hält alle Infos, die wir zum Zeichnen und Verwalten brauchen.
struct ItemRegistry {
    Item* item;           // Zeiger (Pointer) auf das eigentliche Objekt (den Bauplan des Items)
    GLuint texture;       // Die ID des geladenen Bildes auf der Grafikkarte
    bool isInInventory;   // Ist das Häkchen in der Liste gesetzt?
    bool rotated;         // Wurde es mit Rechtsklick gedreht?
    ItemMetadata meta;    // NEU: Unsere Details für das Info-Fenster
};

// Lädt ein Bild von der Festplatte und schiebt es in den Grafikspeicher (VRAM).
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int image_width = 0, image_height = 0, channels = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &channels, 4);
    if (image_data == NULL) return false;

    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;
    return true;
}

// Die Klasse Inventory verwaltet das logische Gitter (Wer liegt wo?)
class Inventory {
public:
    int columns, rows;
    std::vector<Item*> items; 
    std::vector<std::vector<Item*>> grid; // Ein 2D-Array, das für jedes Kästchen merkt, welches Item dort liegt.

    // Der Konstruktor: Wird aufgerufen, wenn das Inventory erschaffen wird (z.B. Inventory briefcase(10,8))
    Inventory(int cols, int rows) : columns(cols), rows(rows) {
        grid.resize(columns, std::vector<Item*>(rows, nullptr));
    }

    // Prüft, ob Platz ist. Gibt 'true' zurück, wenn alle benötigten Kästchen leer (nullptr) sind.
    bool canPlaceItem(Item* item, int startX, int startY) {
        if (startX < 0 || startY < 0 || startX + item->gridWidth > columns || startY + item->gridHeight > rows) return false; 
        for (int x = startX; x < startX + item->gridWidth; x++) {
            for (int y = startY; y < startY + item->gridHeight; y++) {
                if (grid[x][y] != nullptr) return false;
            }
        }
        return true;
    }

    // Versucht das Item zu platzieren. Wenn Platz ist, werden die Zellen im Grid mit dem Zeiger des Items belegt.
    bool addItemAt(Item* item, int startX, int startY) {
        if (canPlaceItem(item, startX, startY)) {
            for (int x = startX; x < startX + item->gridWidth; x++) {
                for (int y = startY; y < startY + item->gridHeight; y++) {
                    grid[x][y] = item;
                }
            }
            item->gridX = startX;
            item->gridY = startY;
            items.push_back(item);
            return true;
        }
        return false;
    }

    // Entfernt ein Item aus dem Grid, indem es die Zellen wieder auf 'nullptr' (leer) setzt.
    void removeItem(Item* item) {
        if (item->gridX != -1) {
            for (int x = item->gridX; x < item->gridX + item->gridWidth; x++) {
                for (int y = item->gridY; y < item->gridY + item->gridHeight; y++) {
                    grid[x][y] = nullptr;
                }
            }
            item->gridX = -1;
            item->gridY = -1;
            for (auto it = items.begin(); it != items.end(); ++it) {
                if (*it == item) { items.erase(it); break; }
            }
        }
    }

    bool moveItem(Item* item, int newX, int newY) {
        int oldX = item->gridX, oldY = item->gridY;
        removeItem(item); 
        if (addItemAt(item, newX, newY)) return true;
        addItemAt(item, oldX, oldY);
        return false;
    }
};

int main() {
    // GLFW startet das Windows-Fenster, OpenGL ist die Grafik-Schnittstelle
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1400, 900, "Resident Evil Inventory", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync an

    // ImGui initialisieren
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 1.0f); 
    style.Colors[ImGuiCol_Border]   = ImVec4(0.3f, 0.3f, 0.3f, 0.5f);

    Inventory briefcase(10, 8);
    std::vector<ItemRegistry> registry;

    // Lambda-Funktion (Eine Funktion in einer Funktion) zum schnellen Registrieren von Items.
    // NEU: Wir übergeben jetzt auch die Metadaten.
    auto registerItem = [&](Item* item, const char* imgPath, ItemMetadata meta) {
        GLuint tex = 0;
        int w, h;
        if (LoadTextureFromFile(imgPath, &tex, &w, &h)) {
            registry.push_back({ item, tex, false, false, meta }); 
        } else {
            registry.push_back({ item, 0, false, false, meta });
        }
    };

    // --- ITEMS REGISTRIEREN (Mit Kategorien und Metadaten) ---
    // Aufbau: Name, X, Y, V0, Kapazität, Bildpfad, {Kategorie, Hersteller, Magazinkapazität}
    registerItem(new Weapon("Beretta M1934", 2, 2, 120, 8), "img/handguns/m1934.png", {"Waffen", "Fabbrica d'Armi Pietro Beretta", 8});
    registerItem(new Weapon("Maxim 9", 3, 2, 140, 10), "img/handguns/Maxim9.png", {"Waffen", "SilencerCo", 15});
    registerItem(new Weapon("P320", 2, 2, 100, 15), "img/handguns/P320.png", {"Waffen", "SIG Sauer", 15});
    registerItem(new Weapon("P232", 2, 2, 90, 8), "img/handguns/P232.png", {"Waffen", "Walther", 8});
    registerItem(new Weapon("RSH-12,", 3, 2, 200, 12), "img/handguns/rsh_12.png", {"Waffen", "RSh-12", 12});
    registerItem(new Weapon("Taurus Judge", 2, 2, 110, 5), "img/handguns/TaurusJudgeLC.png", {"Waffen", "Taurus", 5});
    registerItem(new Weapon("TC Encore Hunter", 3, 2, 180, 1), "img/handguns/TC_Encore_Hunter.png", {"Waffen", "Thompson/Center", 1});
    registerItem(new Weapon("H&K VP70", 3, 2, 130, 10), "img/handguns/VP-70.png", {"Waffen", "Heckler & Koch", 10});

    registerItem(new Weapon("Airtronic RPG-7", 4, 2, 500, 1), "img/rifles/Airtronic_RPG7.png", {"Waffen", "Airtronic", 1});
    registerItem(new Weapon("M990 spx", 4, 2, 300, 15), "img/rifles/M990_spx.png", {"Waffen", "Beretta", 15});
    registerItem(new Weapon("MCX Virtus", 4, 2, 220, 30), "img/rifles/MCX_VIRTUS_14.png", {"Waffen", "SIG Sauer", 30});
    registerItem(new Weapon("Serbu Super Shorty", 4, 2, 600, 3), "img/rifles/Mossberg_Serbu.png", {"Waffen", "Serbu Firearms", 3});
    registerItem(new Weapon("M1A1 Thompson", 4, 2, 250, 20), "img/rifles/M1A_JAE-100.png", {"Waffen", "Auto-Ordnance", 30});
    registerItem(new Weapon("Remington 870", 4, 2, 150, 8), "img/rifles/Remington870Short.png", {"Waffen", "Remington", 8});
    registerItem(new Weapon("Winchester M70Alaskan", 4, 2, 350, 5), "img/rifles/WinchesterM70Alaskan.png", {"Waffen", "Winchester", 5});
    
    registerItem(new Weapon("Evo 3 A1", 4, 2, 200, 30), "img/submachine/Evo_3_A1.png", {"Waffen", "Brügger & Thomet", 30});
    registerItem(new Weapon("MPA30DMG", 4, 2, 250, 30), "img/submachine/MPA30DMG.png", {"Waffen", "MasterPiece Arms", 30});
    registerItem(new Weapon("Uzi", 4, 2, 180, 32), "img/submachine/Uzi-1.png", {"Waffen", "Izhmash", 32});
    
    registerItem(new Ammo("Pistolenmunition", 2, 2, "9mm", 30), "img/ammo/9mm.png", {"Munition", "Umarex", 30});
    registerItem(new Ammo("Schrotmunition", 2, 2, "12 Gauge", 15), "img/ammo/shells.png", {"Munition", "Winchester", 15});
    registerItem(new Ammo("Gewehrmunition 308", 2, 2, "7.62x51mm", 20), "img/ammo/308.png", {"Munition", "Remington", 20});
    registerItem(new Ammo("Gewehrmunition hornet", 2, 2, "22 Hornet", 25), "img/ammo/hornet.png", {"Munition", "Winchester", 25});
    registerItem(new Ammo("Pistolenmunition Requiem", 2, 2, "9mm", 15), "img/ammo/requiem.png", {"Munition", "Umarex", 15});
    
    // Beispiel für ein Misc Item (Angenommen du hast eine Klasse 'MiscItem' oder nutzt einfach Item)
    registerItem(new Item("Grünes Kraut", 2, 2), "img/misc/herb.png", {"Misc", "Arklay Mountains", 0});

    // Hauptschleife: Läuft solange das Fenster offen ist (ca. 60 mal pro Sekunde)
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents(); // Prüft auf Maus- und Tastatureingaben
        
        // Sagt ImGui, dass wir ein neues Bild (Frame) zeichnen wollen
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Inventar Management", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // --- SEITENLEISTE (BeginChild erstellt einen abgetrennten Bereich im Fenster) ---
        ImGui::BeginChild("ItemSidebar", ImVec2(280, 650), true);
        ImGui::Text("Ausrüstung:");
        ImGui::Separator();
        
        // --- BUTTON: KOFFER LEEREN ---
        // PushStyleColor ändert kurzzeitig die Farbe für das nächste Element
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.6f, 0.1f, 0.1f, 1.0f));       // Dunkelrot
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f)); // Helles Rot beim Hovern
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.0f, 0.0f, 1.0f));  // Klick-Farbe
        
        // ImVec2(-1, 0) bedeutet: Mach den Button so breit wie das Fenster (-1) und Standardhöhe (0)
        if (ImGui::Button("Koffer leeren", ImVec2(-1, 0))) {
            for (auto& entry : registry) {
                entry.isInInventory = false;      // Haken raus
                briefcase.removeItem(entry.item); // Aus dem Gitter löschen
            }
        }
        ImGui::PopStyleColor(3); // Farben wieder auf Standard zurücksetzen! Wichtig!
        ImGui::Spacing();

        // --- KATEGORIEN (Aufklapp-Menüs) ---
        // Liste aller Kategorien, die wir definiert haben
        const char* categories[] = {"Waffen", "Munition", "Misc"};
        
        for (const char* cat : categories) {
            // ImGuiTreeNodeFlags_DefaultOpen sorgt dafür, dass die Menüs anfangs aufgeklappt sind
            if (ImGui::CollapsingHeader(cat, ImGuiTreeNodeFlags_DefaultOpen)) {
                for (auto& entry : registry) {
                    if (entry.meta.category == cat) {
                        // ImGui::Checkbox zeichnet eine Box und verknüpft sie mit dem Boolean 'isInInventory'.
                        // Wenn der User klickt, ändert ImGui automatisch den Wert der Variable!
                        if (ImGui::Checkbox(entry.item->name.c_str(), &entry.isInInventory)) {
                            if (entry.isInInventory) {
                                bool placed = false;
                                // Versuche einen freien Platz zu finden
                                for (int y = 0; y <= briefcase.rows - entry.item->gridHeight && !placed; y++) {
                                    for (int x = 0; x <= briefcase.columns - entry.item->gridWidth && !placed; x++) {
                                        if (briefcase.addItemAt(entry.item, x, y)) placed = true;
                                    }
                                }
                                if (!placed) entry.isInInventory = false; // Kein Platz? Haken wieder raus!
                            } else {
                                briefcase.removeItem(entry.item);
                            }
                        }
                    }
                }
            }
        }
        ImGui::EndChild(); // Beendet die Seitenleiste
        
        ImGui::SameLine(); // Sagt ImGui: "Zeichne das nächste Element rechts daneben, nicht darunter!"

        // --- AKTENKOFFER BEREICH ---
        ImGui::BeginGroup(); // Gruppiert alles folgende, damit es ordentlich formatiert bleibt
        
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (10 * 75.0f) / 2 - 20.0f);
        ImGui::Text("ITEMS");
        ImGui::Spacing();
        
        float slotSize = 75.0f;
        
        // GetWindowDrawList() ist wie ein Pinsel. Damit kannst du frei Rechtecke, Linien und Bilder malen,
        // unabhängig vom normalen ImGui Layout-System.
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec2 gridStartPos = ImGui::GetCursorScreenPos(); // Absolute X/Y Koordinaten auf dem Monitor

        // 1. DAS GITTER ZEICHNEN
        for (int y = 0; y < briefcase.rows; ++y) {
            for (int x = 0; x < briefcase.columns; ++x) {
                ImVec2 cellMin = ImVec2(gridStartPos.x + x * slotSize, gridStartPos.y + y * slotSize);
                ImVec2 cellMax = ImVec2(cellMin.x + slotSize, cellMin.y + slotSize);

                draw_list->AddRectFilled(cellMin, cellMax, IM_COL32(20, 20, 20, 200));
                draw_list->AddRect(cellMin, cellMax, IM_COL32(80, 80, 80, 150));

                ImGui::SetCursorScreenPos(cellMin);
                ImGui::SetNextItemAllowOverlap(); 
                ImGui::PushID(y * briefcase.columns + x); // PushID ist wichtig, wenn man viele gleiche unsichtbare Elemente hat
                ImGui::InvisibleButton("##slot", ImVec2(slotSize, slotSize));

                // Drag & Drop Logik
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("INV_ITEM")) {
                        Item* droppedItem = *(Item**)payload->Data;
                        briefcase.moveItem(droppedItem, x, y);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID(); // Muss immer nach einem PushID aufgerufen werden!
            }
        }

        // 2. DIE ITEMS ZEICHNEN & INFO-FENSTER (Tooltip)
        for (auto& entry : registry) {
            if (entry.item->gridX == -1) continue; // Item ist nicht im Koffer

            ImVec2 itemMin = ImVec2(gridStartPos.x + entry.item->gridX * slotSize, gridStartPos.y + entry.item->gridY * slotSize);
            ImVec2 itemMax = ImVec2(itemMin.x + entry.item->gridWidth * slotSize, itemMin.y + entry.item->gridHeight * slotSize);

            ImGui::SetCursorScreenPos(itemMin);
            ImGui::PushID(entry.item);
            ImGui::SetNextItemAllowOverlap();
            ImGui::InvisibleButton("##item", ImVec2(itemMax.x - itemMin.x, itemMax.y - itemMin.y));

            // Prueft, ob die Maus genau jetzt über diesem gezeichneten Item ist
            bool isHovered = ImGui::IsItemHovered();

            // --- NEU: INFO-FENSTER (Tooltip) ---
            if (isHovered) {
                // BeginTooltip erstellt ein kleines Fenster, das der Maus folgt
                ImGui::BeginTooltip();
                
                // ImGui::TextColored erlaubt uns, dem Text eine bestimmte Farbe (RGBA) zu geben.
                ImGui::TextColored(ImVec4(0.8f, 0.9f, 0.4f, 1.0f), "%s", entry.item->name.c_str());
                ImGui::Separator();
                
                // Details aus unseren Metadaten anzeigen
                ImGui::Text("Kategorie: %s", entry.meta.category.c_str());
                ImGui::Text("Hersteller: %s", entry.meta.manufacturer.c_str());
                
                // Nur anzeigen, wenn die Kapazität größer als 0 ist
                if (entry.meta.capacity > 0) {
                    ImGui::Text("Kapazität: %d Schuss", entry.meta.capacity);
                }
                
                // Man könnte hier auch noch Item-Platzbedarf berechnen:
                ImGui::TextDisabled("Größe: %dx%d Slots", entry.item->gridWidth, entry.item->gridHeight);
                
                ImGui::EndTooltip();
            }

            // Rechtsklick Rotation
            if (isHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                int oldX = entry.item->gridX, oldY = entry.item->gridY;
                int oldW = entry.item->gridWidth, oldH = entry.item->gridHeight;
                briefcase.removeItem(entry.item); 
                entry.item->gridWidth = oldH; entry.item->gridHeight = oldW;
                if (briefcase.addItemAt(entry.item, oldX, oldY)) {
                    entry.rotated = !entry.rotated; 
                } else {
                    entry.item->gridWidth = oldW; entry.item->gridHeight = oldH;
                    briefcase.addItemAt(entry.item, oldX, oldY);
                }
            }

            // Drag Source (Element anheben)
            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("INV_ITEM", &entry.item, sizeof(Item*));
                if (entry.texture != 0) ImGui::Image((ImTextureID)(intptr_t)entry.texture, ImVec2(60, 60)); 
                ImGui::EndDragDropSource();
            }

            // Hintergrund zeichnen
            ImU32 bgColor = isHovered ? IM_COL32(70, 90, 60, 200) : IM_COL32(35, 35, 40, 200);
            draw_list->AddRectFilled(itemMin, itemMax, bgColor);

            // Textur (Bild) zeichnen
            if (entry.texture != 0) {
                float imgPad = 5.0f; 
                ImVec2 imgMin = ImVec2(itemMin.x + imgPad, itemMin.y + imgPad);
                ImVec2 imgMax = ImVec2(itemMax.x - imgPad, itemMax.y - imgPad);

                if (!entry.rotated) {
                    draw_list->AddImageQuad((ImTextureID)(intptr_t)entry.texture, imgMin, ImVec2(imgMax.x, imgMin.y), imgMax, ImVec2(imgMin.x, imgMax.y));
                } else {
                    draw_list->AddImageQuad((ImTextureID)(intptr_t)entry.texture, imgMin, ImVec2(imgMax.x, imgMin.y), imgMax, ImVec2(imgMin.x, imgMax.y), ImVec2(0, 1), ImVec2(0, 0), ImVec2(1, 0), ImVec2(1, 1));
                }
            }

            // Rahmen zeichnen (bei Hovern gelb-grünlich)
            if (isHovered) {
                draw_list->AddRect(itemMin, itemMax, IM_COL32(200, 230, 150, 255), 0.0f, 0, 2.0f);
            } else {
                draw_list->AddRect(itemMin, itemMax, IM_COL32(100, 100, 100, 255));
            }

            ImGui::PopID();
        }

        // Dummy zwingt ImGui dazu, Platz freizuhalten, damit wir nicht über den Fensterrand hinaus malen.
        ImGui::SetCursorScreenPos(gridStartPos);
        ImGui::Dummy(ImVec2(briefcase.columns * slotSize, briefcase.rows * slotSize));

        ImGui::EndGroup(); // Beendet den Aktenkoffer-Bereich
        ImGui::Separator();
        ImGui::Text("Status: RE Look aktiv. | %d Items im Koffer | Rechtsklick zum Rotieren", (int)briefcase.items.size());
        ImGui::End();

        // Rendern an die Grafikkarte schicken
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); 
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Beim Beenden den Speicher aufräumen
    for(auto& e : registry) delete e.item;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}