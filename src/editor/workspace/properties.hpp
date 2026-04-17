#pragma once
#include <imgui.h>
#include <vector>
#include <memory>

#include "editor/interface/i_interface.hpp"

class Properties {
private:
    std::vector<std::shared_ptr<IInterface>> interfaces;

public:
    void AddInterface(const std::shared_ptr<IInterface>& interface) {
        interfaces.push_back(interface);
    }

    void Draw() const {
        ImGui::SetNextWindowSize(ImVec2(560, 720), ImGuiCond_::ImGuiCond_FirstUseEver);
        ImGui::Begin("Properties");

        for (auto& interface : interfaces) {
            interface->Draw();
        }

        ImGui::End();
    }
};
