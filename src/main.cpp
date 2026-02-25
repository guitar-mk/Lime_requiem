#include <iostream>
#include <vector>

// ImGui und Backend-Bibliotheken
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GLFW/glfw3.h> // Das Fenster-Framework

// Deine eigenen Klassen
#include "Weapon.h"
#include "WeaponTypes.h"

// Einfache Inventar-Klasse für unseren Test
class Inventory {
public:
    std::vector<Item*> items;
    void addItem(Item* newItem) { items.push_back(newItem); }
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

    // 4. Dein Backend-Inventar aufbauen (wie vorher!)
    Inventory briefcase;
    briefcase.addItem(new Weapon("Alligator Snapper (P320)", 2, 2, 150, 15));
    briefcase.addItem(new Weapon("Serbu Super Shorty", 4, 2, 600, 3));
    briefcase.addItem(new Ammo("Pistolenmunition", 1, 1, "9mm", 30));

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

        // Wir loopen durch dein Backend und zeichnen Buttons dafür
        for (Item* item : briefcase.items) {
            // Wir tun so, als wären die Buttons das Raster
            ImGui::Button(item->name.c_str(), ImVec2(item->gridWidth * 50.0f, item->gridHeight * 50.0f));
            
            // Ein kleiner Tooltip (Info), wenn man mit der Maus drüberfährt
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("Belegt %dx%d Slots", item->gridWidth, item->gridHeight);
            }
            
            ImGui::SameLine(); // Alles nebeneinander zeichnen
        }
        
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