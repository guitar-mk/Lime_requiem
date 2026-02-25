#include <iostream>
#include <vector>

// stb_image.h für png->openGL->ImGui
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ImGui und Backend-Bibliotheken
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Das Fenster-Framework

// Deine eigenen Klassen
#include "Weapon.h"
#include "WeaponTypes.h"

// Laden des pngs als OpenGL-Texturen (für die Item-Icons)
bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height) {
    int image_width = 0, image_height = 0, channels = 0;
    
    // stb_image entpackt das PNG
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, &channels, 4);
    if (image_data == NULL) return false;

    // OpenGL eine neue Textur erstellen lassen
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Bildeinstellungen (damit es beim Skalieren nicht verschwommen wird)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Die rohen Pixel auf die Grafikkarte laden
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    
    // Den Arbeitsspeicher wieder freigeben (stb_image aufräumen)
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    return true;
}

class Inventory {
public:
    int columns;
    int rows;
    std::vector<Item*> items; // Liste für die Items an sich
    
    // Das echte 2D-Raster (Speichert Zeiger auf Items. Wenn leer, dann nullptr)
    std::vector<std::vector<Item*>> grid;

    Inventory(int cols, int rows) : columns(cols), rows(rows) {
        // Wir bauen das Raster auf und füllen alles mit "leer" (nullptr)
        grid.resize(columns, std::vector<Item*>(rows, nullptr));
    }

    // 1. Die Kollisions-Prüfung: Passt das Item hier hin?
    bool canPlaceItem(Item* item, int startX, int startY) {
        // Passt es überhaupt in den Koffer oder ragt es über den Rand hinaus?
        if (startX < 0 || startY < 0 || 
            startX + item->gridWidth > columns || 
            startY + item->gridHeight > rows) {
            return false; 
        }

        // Prüfen, ob auch wirklich JEDES der benötigten Kästchen leer ist
        for (int x = startX; x < startX + item->gridWidth; x++) {
            for (int y = startY; y < startY + item->gridHeight; y++) {
                if (grid[x][y] != nullptr) {
                    return false; // Da liegt schon was! Platz belegt.
                }
            }
        }
        return true; // Alles frei!
    }

    // 2. Das Item endgültig in den Koffer legen
    bool addItemAt(Item* item, int startX, int startY) {
        if (canPlaceItem(item, startX, startY)) {
            // Wir belegen die Kästchen in unserer Matrix mit dem Item
            for (int x = startX; x < startX + item->gridWidth; x++) {
                for (int y = startY; y < startY + item->gridHeight; y++) {
                    grid[x][y] = item;
                }
            }
            // Wir speichern die Koordinaten im Item selbst
            item->gridX = startX;
            item->gridY = startY;
            items.push_back(item);
            return true;
        }
        std::cout << "Fehler: Kein Platz fuer " << item->name << " an Position " << startX << "," << startY << "\n";
        return false;
    }
};

int main() {
    // 1. GLFW initialisieren (Fenster-System starten)
    if (!glfwInit()) return -1;
    
    // Wir nutzen OpenGL 3.0
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // 2. Das eigentliche Fenster erstellen (1280x720 Pixel)
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Resident Evil Inventar", NULL, NULL);
    if (window == NULL) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // V-Sync aktivieren (60 FPS)

    // 3. ImGui initialisieren
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark(); // Schön dunkler RE-Look

    // Die Brücke zwischen ImGui, GLFW und OpenGL schlagen
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // 4. Dein Backend-Inventar aufbauen
    Inventory briefcase(10, 8); // Koffer ist 10 breit, 8 hoch

    Weapon* pistol = new Weapon("Alligator Snapper", 2, 2, 150, 15);
    Weapon* shotgun = new Weapon("Serbu Super Shorty", 4, 2, 600, 3);
    Ammo* ammo = new Ammo("Pistolenmunition", 1, 1, "9mm", 30);

    // Pistole oben links in die Ecke (X:0, Y:0)
    briefcase.addItemAt(pistol, 0, 0); 
    
    // Schrotflinte daneben (X:2, Y:0) - da die Pistole 2 breit ist
    briefcase.addItemAt(shotgun, 2, 0); 
    
    // Munition in die nächste Zeile unter die Pistole (X:0, Y:2)
    briefcase.addItemAt(ammo, 0, 2);
    GLuint texPistol, texShotgun, texAmmo;
    int w, h; // Platzhalter für die echte Bildgröße in Pixeln
    
    // Bilder laden (Pfade müssen zu deinem img-Ordner passen!)
    LoadTextureFromFile("img/pistol.png", &texPistol, &w, &h);
    LoadTextureFromFile("img/shotgun.png", &texShotgun, &w, &h);
    LoadTextureFromFile("img/ammo.png", &texAmmo, &w, &h);
    
    // 5. DER GAME-LOOP (Läuft solange, bis du das Fenster schließt)
    while (!glfwWindowShouldClose(window)) {
        // Events abfragen (Mausklicks, Tastatur)
        glfwPollEvents();

        // Neuen Frame für ImGui starten
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // --- HIER PASSIERT DIE MAGIE: Das ImGui Fenster ---
        ImGui::Begin("Aktenkoffer (Inventar)", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImGui::Text("Dein Kofferinhalt:");
ImGui::Separator();

// 1. Einstellungen für das Raster
float slotSize = 100.0f;
ImVec2 gridStartPos = ImGui::GetCursorPos();

// 2. Das leere Hintergrund-Raster zeichnen
ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
for (int y = 0; y < briefcase.rows; ++y) {
for (int x = 0; x < briefcase.columns; ++x) {
ImGui::SetCursorPos(ImVec2(gridStartPos.x + x * slotSize, gridStartPos.y + y * slotSize));
ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
ImGui::Button("##grid", ImVec2(slotSize, slotSize));
ImGui::PopStyleColor();
}
}
ImGui::PopStyleVar();

// 3. Die Items an ihre exakte Position zeichnen
for (Item* item : briefcase.items) {
if (item->gridX == -1 || item->gridY == -1) continue;

}

ImGui::SetCursorPos(ImVec2(gridStartPos.x, gridStartPos.y + briefcase.rows * slotSize + 20));
ImGui::Text("Status: Resident Evil Inventar geladen.");

ImGui::End();
        // --- ENDE DER MAGIE ---

        // Bildschirm schwarz wischen und ImGui rendern
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Hintergrundfarbe
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Das fertige Bild auf den Monitor werfen
        glfwSwapBuffers(window);
    }

    // 6. Aufräumen, wenn das Fenster geschlossen wird
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}