#pragma once
#include <imgui.h>
#include <iostream>
#include <memory>

#include "i_interface.hpp"
#include "editor/workspace/workspaces.hpp"
#include "util/screenshot.hpp"

class GlobalActionsInterface : public IInterface {
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
    }

    virtual ~GlobalActionsInterface() override = default;
};
