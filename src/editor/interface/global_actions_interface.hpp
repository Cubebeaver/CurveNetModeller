#pragma once
#include <imgui.h>
#include <iostream>
#include <memory>
#include <tinyfiledialogs.h>

#include "i_interface.hpp"
#include "editor/workspace/workspaces.hpp"
#include "util/screenshot.hpp"

class GlobalActionsInterface : public IInterface {
private:
    const char* filterPatterns[1] = { "*.cnm" };

public:
    GlobalActionsInterface() { }

    virtual void Draw() override {
        ImGui::SeparatorText("Tests");
        if (ImGui::Button("Hello World!")) {
            std::cout << "Hello World!" << std::endl;
        }
        ImGui::SameLine();
        if (auto vp = Workspaces::viewport.lock()) {
            if (ImGui::Button("Save viewport screenshot")) {
                CaptureScreenshot(vp->viewportBuffer->fbo, vp->viewportBuffer->Width, vp->viewportBuffer->Height);
            }
        }

        ImGui::SeparatorText("Save/Load");
        if (ImGui::Button("Save")) {
            const char* saveFile = tinyfd_saveFileDialog(
                "Save as...",
                "new document.cnm",
                1,
                filterPatterns,
                "CurveNetModeller project files"
            );

            if (saveFile) {
                std::cout << "[Save] A fájlt ide kell menteni:\n" << saveFile << "\n";
            } else {
                std::cout << "[Save] A mentés megszakítva.\n";
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load")) {
            const char* singleFile = tinyfd_openFileDialog(
                "Válassz ki egy szöveges fájlt",
                "./output/",
                1,
                filterPatterns,
                "CurveNetModeller project files",
                0
            );

            if (singleFile) {
                std::cout << "[Open] Kiválasztott fájl:\n" << singleFile << "\n\n";
            } else {
                std::cout << "[Open] A felhasználó bezárta az ablakot.\n\n";
            }
        }
    }

    virtual ~GlobalActionsInterface() override = default;
};
