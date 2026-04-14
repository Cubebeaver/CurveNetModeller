#pragma once
#include <imgui.h>
#include <vector>

#include "editor/interface/i_interface.hpp"

class Properties {
private:
    std::vector<IInterface> interfaces;

public:
    void AddInterface(IInterface interface) {
        interfaces.push_back(interface);
    }

    void Draw() const {
        ImGui::SetNextWindowSize(ImVec2(560, 720), ImGuiCond_FirstUseEver);
        ImGui::Begin("Properties");

        for (auto interface : interfaces) {
            interface.Draw();
        }

        ImGui::End();
    }
};
