#include <iostream>
#include <vector>

// stb_image.h für png->openGL->ImGui
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

// Laden des pngs als OpenGL-Texturen
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

class Inventory {
public:
    int columns;
    int rows;
    std::vector<Item*> items; 
    std::vector<std::vector<Item*>> grid;

    Inventory(int cols, int rows) : columns(cols), rows(rows) {
        grid.resize(columns, std::vector<Item*>(rows, nullptr));
    }

    bool canPlaceItem(Item* item, int startX, int startY) {
        if (startX < 0 || startY < 0 || 
            startX + item->gridWidth > columns || 
            startY + item->gridHeight > rows) {
            return false; 
        }
        for (int x = startX; x < startX + item->gridWidth; x++) {
            for (int y = startY; y < startY + item->gridHeight; y++) {
                if (grid[x][y] != nullptr) return false;
            }
        }
        return true;
    }

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

    // Neues Feature: Ein Item aus dem Raster entfernen
    void removeItem(Item* item) {
        if (item->gridX != -1 && item->gridY != -1) {
            for (int x = item->gridX; x < item->gridX + item->gridWidth; x++) {
                for (int y = item->gridY; y < item->gridY + item->gridHeight; y++) {
                    grid[x][y] = nullptr; // Kästchen wieder freigeben
                }
            }
            item->gridX = -1;
            item->gridY = -1;
        }
    }

    // Neues Feature: Ein Item an einen neuen Ort bewegen
    bool moveItem(Item* item, int newX, int newY) {
        int oldX = item->gridX;
        int oldY = item->gridY;
            
            // Erstmal virtuell aufheben, damit es nicht mit sich selbst kollidiert
            removeItem(item); 
            
            if (canPlaceItem(item, newX, newY)) {
                // Platz ist frei! Wir legen es an die neue Stelle
                for (int x = newX; x < newX + item->gridWidth; x++) {
                    for (int y = newY; y < newY + item->gridHeight; y++) {
                        grid[x][y] = item;
                    }
                }
                item->gridX = newX;
                item->gridY = newY;
                return true;
            } else {
                // Platz ist belegt! Wir legen es zurück an den alten Ort
                for (int x = oldX; x < oldX + item->gridWidth; x++) {
                    for (int y = oldY; y < oldY + item->gridHeight; y++) {
                        grid[x][y] = item;
                    }
                }
                item->gridX = oldX;
                item->gridY = oldY;
                return false;
            }
        }
};

int main() {
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 900, "Resident Evil Inventar", NULL, NULL);
    if (window == NULL) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::StyleColorsDark();

    // Backend-Setup
    Inventory briefcase(10, 8);
    Weapon* pistol = new Weapon("Alligator Snapper", 2, 2, 150, 15);
    Weapon* shotgun = new Weapon("Serbu Super Shorty", 4, 2, 600, 3);
    Weapon* m1934 = new Weapon("Beretta M1934", 2, 2, 120, 8);
    Weapon* mac10 = new Weapon("MAC-10", 3, 2, 200, 30);
    Weapon* rsh12 = new Weapon("RSH-12", 3, 2, 800, 2);
    Ammo* ammo = new Ammo("Pistolenmunition", 1, 1, "9mm", 30);

    briefcase.addItemAt(pistol, 0, 0); 
    briefcase.addItemAt(m1934, 4, 0);
//    briefcase.addItemAt(mac10, 0, 4);
    briefcase.addItemAt(rsh12, 5, 4);
//    briefcase.addItemAt(shotgun, 6, 0); 
    briefcase.addItemAt(ammo, 0, 2);

    // Texturen laden
    GLuint texPistol, texShotgun, texAmmo, texMac10, texRsh12, texM1923;
    int w, h;
    LoadTextureFromFile("img/pistol2.png", &texPistol, &w, &h);
    LoadTextureFromFile("img/shotgun.png", &texShotgun, &w, &h);
    LoadTextureFromFile("img/ammo.png", &texAmmo, &w, &h);
    LoadTextureFromFile("img/mac10.png", &texMac10, &w, &h);
    LoadTextureFromFile("img/rsh12.png", &texRsh12, &w, &h);
    LoadTextureFromFile("img/M1934.png", &texM1923, &w, &h);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Aktenkoffer (Inventar)", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Dein Kofferinhalt:");
        ImGui::Separator();

        // ----------------------------------------------------
        // HIER STELLST DU DIE GRÖßE DES INVENTARS EIN
        // ----------------------------------------------------
        float slotSize = 80.0f; // <--- Mach es hier größer oder kleiner (z.B. 64.0f oder 100.0f)
        ImVec2 gridStartPos = ImGui::GetCursorPos();

        // 1. Hintergrund-Raster zeichnen (jetzt als DROP-Ziel!)
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int y = 0; y < briefcase.rows; ++y) {
            for (int x = 0; x < briefcase.columns; ++x) {
                ImGui::SetCursorPos(ImVec2(gridStartPos.x + x * slotSize, gridStartPos.y + y * slotSize));
                ImGui::PushID(y * briefcase.columns + x);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.15f, 0.15f, 0.15f, 0.7f));
                
                // WICHTIG: Erlaubt, dass die Waffe über dem Kästchen liegt und Klicks fangen kann
                ImGui::SetNextItemAllowOverlap();
                
                ImGui::Button("##slot", ImVec2(slotSize, slotSize));
                ImGui::PopStyleColor();

                // DRAG & DROP TARGET: Nimmt Items an, die hier fallen gelassen werden
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ITEM_PAYLOAD")) {
                        // Wir haben ein Item gefangen!
                        Item* droppedItem = *(Item**)payload->Data;
                        // Versuche es an die x/y Koordinate des aktuellen Kästchens zu legen
                        briefcase.moveItem(droppedItem, x, y);
                    }
                    ImGui::EndDragDropTarget();
                }

                ImGui::PopID();
            }
        }
        ImGui::PopStyleVar();

        // 2. Die Items obendrüber zeichnen (jetzt als DRAG-Quelle!)
        float padding = 4.0f; // Abstand zum Rand in Pixeln

        for (Item* item : briefcase.items) {
            if (item->gridX == -1) continue;

            // Position berechnen MIT PADDING (leicht nach unten rechts versetzt)
            ImVec2 pos = ImVec2(gridStartPos.x + item->gridX * slotSize + padding, 
                                gridStartPos.y + item->gridY * slotSize + padding);
            ImGui::SetCursorPos(pos);

            GLuint tex = 0;
            if (item->name == "Alligator Snapper") tex = texPistol;
            else if (item->name == "Serbu Super Shorty") tex = texShotgun;
            else if (item->name == "Pistolenmunition") tex = texAmmo;
            else if (item->name == "MAC-10") tex = texMac10;
            else if (item->name == "RSH-12") tex = texRsh12;
            else if (item->name == "Beretta M1934") tex = texM1923;

            // Größe berechnen ABZÜGLICH PADDING (von beiden Seiten)
            ImVec2 size = ImVec2((item->gridWidth * slotSize) - (padding * 2), 
                                 (item->gridHeight * slotSize) - (padding * 2));
            
            ImGui::PushID(item); 
            
            // Damit das Item selbst auch Überlappungen erlaubt (falls man es wieder anfasst)
            ImGui::SetNextItemAllowOverlap();

            if (tex != 0) {
                // Tint color auf Weiß (Standard), Border color auf Transparent
                ImGui::ImageButton(item->name.c_str(), (ImTextureID)(intptr_t)tex, size, ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,0), ImVec4(1,1,1,1));
            } else {
                ImGui::Button(item->name.c_str(), size);
            }

            // DRAG & DROP SOURCE: Wenn man dieses Bild anklickt und zieht
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                // Sende einen Pointer auf dieses Item
                ImGui::SetDragDropPayload("ITEM_PAYLOAD", &item, sizeof(Item*));
                
                // Vorschau-Bild rendern, das an der Maus klebt
                if (tex != 0) {
                    ImGui::Image((ImTextureID)(intptr_t)tex, size);
                } else {
                    ImGui::Text("Verschiebe %s", item->name.c_str());
                }
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s (%dx%d)", item->name.c_str(), item->gridWidth, item->gridHeight);
            }
            ImGui::PopID();
        } // <--- Diese Klammer hat vorher gefehlt/war verrutscht

        // Footer
        ImGui::SetCursorPos(ImVec2(gridStartPos.x, gridStartPos.y + briefcase.rows * slotSize + 10));
        ImGui::Separator();
        ImGui::Text("Status: Resident Evil Inventar bereit.");
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}