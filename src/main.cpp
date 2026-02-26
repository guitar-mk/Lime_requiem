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

struct ItemRegistry {
    Item* item;
    GLuint texture;
    bool isInInventory;
};

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
    int columns, rows;
    std::vector<Item*> items; 
    std::vector<std::vector<Item*>> grid;

    Inventory(int cols, int rows) : columns(cols), rows(rows) {
        grid.resize(columns, std::vector<Item*>(rows, nullptr));
    }

    bool canPlaceItem(Item* item, int startX, int startY) {
        if (startX < 0 || startY < 0 || 
            startX + item->gridWidth > columns || 
            startY + item->gridHeight > rows) return false; 

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
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1400, 900, "Resident Evil Inventory Manager", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
    ImGui::StyleColorsDark();

    Inventory briefcase(10, 8);
    std::vector<ItemRegistry> registry;

    auto registerItem = [&](Item* item, const char* imgPath) {
        GLuint tex = 0;
        int w, h;
        if (LoadTextureFromFile(imgPath, &tex, &w, &h)) {
            registry.push_back({ item, tex, false });
        } else {
            std::cout << "[WARNUNG] Bild konnte nicht geladen werden: " << imgPath << std::endl;
            registry.push_back({ item, 0, false });
        }
    };

    // --- ALLE WAFFEN LADEN ---
    registerItem(new Weapon("Beretta M1934", 2, 2, 120, 8),      "img/handguns/m1934.png");
    registerItem(new Weapon("Maxim 9", 3, 2, 140, 10),           "img/handguns/Maxim9.png");
    registerItem(new Weapon("SIG Sauer P232", 2, 2, 130, 7),     "img/handguns/P232.png");
    registerItem(new Weapon("Alligator Snapper", 2, 2, 150, 15), "img/handguns/P320.png");
    registerItem(new Weapon("Taurus PT92", 2, 2, 140, 17),       "img/handguns/TaurusJudgeLC.png");
    registerItem(new Weapon("Taurus TC Encore", 3, 2, 160, 6),   "img/handguns/TC_Encore_Hunter.png");
    registerItem(new Weapon("HK VP70", 3, 2, 170, 18),           "img/handguns/VP-70.png");

    registerItem(new Weapon("Evo3", 3, 2, 180, 25),              "img/submaschine/Evo_3_A1.png");
    registerItem(new Weapon("MPA30", 3, 2, 190, 30),             "img/submaschine/MPA30DMG.png");
    registerItem(new Weapon("Uzi", 3, 2, 170, 32),               "img/submaschine/Uzi-1.png");

    registerItem(new Weapon("RPG-7", 4, 2, 1000, 1),             "img/rifles/Airtronic_RPG7.png");
    registerItem(new Weapon("M1A1 Thompson", 4, 2, 250, 20),     "img/rifles/M1A_JAE-100.png");
    registerItem(new Weapon("M990", 4, 2, 300, 15),              "img/rifles/M990_spx.png");
    registerItem(new Weapon("Sig MCX", 4, 2, 220, 25),           "img/rifles/MCX_VIRTUS_14.png");
    registerItem(new Weapon("Serbu Super Shorty", 4, 2, 600, 3), "img/rifles/Mossberg_Serbu.png");
    
    // HIER WAR DER FEHLER: Großes 'S' bei Short
    registerItem(new Weapon("Remington 870", 4, 2, 550, 5),      "img/rifles/Remington870Short.png");
    registerItem(new Weapon("Winchester 1897", 4, 2, 500, 5),    "img/rifles/WinchesterM70Alaskan.png");

    registerItem(new Ammo("Pistolenmunition", 1, 1, "9mm", 30),  "img/ammo/ammo.png");

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
        ImGui::Begin("Inventar Management", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

        // --- SEITENLEISTE ---
        ImGui::BeginChild("ItemSidebar", ImVec2(250, 650), true);
        ImGui::Text("Ausrüstung:");
        ImGui::Separator();
        
        for (auto& entry : registry) {
            if (ImGui::Checkbox(entry.item->name.c_str(), &entry.isInInventory)) {
                if (entry.isInInventory) {
                    bool placed = false;
                    for (int y = 0; y <= briefcase.rows - entry.item->gridHeight && !placed; y++) {
                        for (int x = 0; x <= briefcase.columns - entry.item->gridWidth && !placed; x++) {
                            if (briefcase.addItemAt(entry.item, x, y)) placed = true;
                        }
                    }
                    if (!placed) entry.isInInventory = false; 
                } else {
                    briefcase.removeItem(entry.item);
                }
            }
        }
        ImGui::EndChild();
        ImGui::SameLine();

        // --- AKTENKOFFER ---
        ImGui::BeginGroup();
        ImGui::Text("Aktenkoffer (Drag & Drop bereit):");
        
        float slotSize = 75.0f;
        ImVec2 gridStartPos = ImGui::GetCursorPos();

        // 1. Gitter-Slots zeichnen
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        for (int y = 0; y < briefcase.rows; ++y) {
            for (int x = 0; x < briefcase.columns; ++x) {
                ImGui::SetCursorPos(ImVec2(gridStartPos.x + x * slotSize, gridStartPos.y + y * slotSize));
                ImGui::PushID(y * briefcase.columns + x);
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.12f, 0.12f, 0.15f, 1.0f));
                
                // WICHTIG: Erlaubt Überlappung, damit das Gitter nicht Drag&Drop blockiert!
                ImGui::SetNextItemAllowOverlap(); 
                
                ImGui::Button("##slot", ImVec2(slotSize, slotSize));
                ImGui::PopStyleColor();

                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("INV_ITEM")) {
                        Item* droppedItem = *(Item**)payload->Data;
                        briefcase.moveItem(droppedItem, x, y);
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::PopID();
            }
        }
        ImGui::PopStyleVar();

        // 2. Items rendern
        float p = 3.0f; 
        for (auto& entry : registry) {
            if (entry.item->gridX == -1) continue;

            ImVec2 itemPos = ImVec2(gridStartPos.x + entry.item->gridX * slotSize + p, 
                                    gridStartPos.y + entry.item->gridY * slotSize + p);
            ImVec2 itemSize = ImVec2((entry.item->gridWidth * slotSize) - (p * 2), 
                                     (entry.item->gridHeight * slotSize) - (p * 2));

            ImGui::SetCursorPos(itemPos);
            ImGui::PushID(entry.item);
            
            // WICHTIG: Damit die Waffe als Drag-Quelle funktioniert, wenn sie über dem Grid liegt
            ImGui::SetNextItemAllowOverlap();

            if (entry.texture != 0) {
                ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)entry.texture, itemSize, ImVec2(0,0), ImVec2(1,1), ImVec4(0,0,0,0), ImVec4(1,1,1,1));
            } else {
                ImGui::Button(entry.item->name.c_str(), itemSize);
            }

            if (ImGui::BeginDragDropSource()) {
                ImGui::SetDragDropPayload("INV_ITEM", &entry.item, sizeof(Item*));
                if (entry.texture != 0) {
                    ImGui::Image((ImTextureID)(intptr_t)entry.texture, ImVec2(60, 60)); 
                }
                ImGui::Text("%s", entry.item->name.c_str());
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s\nGröße: %dx%d", entry.item->name.c_str(), entry.item->gridWidth, entry.item->gridHeight);
            }
            ImGui::PopID();
        }

        ImGui::EndGroup();
        ImGui::Separator();
        ImGui::Text("Status: Alle Bilder eingebunden. | %d Items im Koffer", (int)briefcase.items.size());
        ImGui::End();

        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.02f, 0.02f, 0.02f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    for(auto& e : registry) delete e.item;
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}