#pragma once
#include <imgui.h>
#include <memory>

#include "IInterface.hpp"
#include "model/point.h"

class PointInterface : public IInterface {
public:
    PointInterface(std::shared_ptr<Point> point) : modelPoint(point) { }

    void Draw() override {
        std::shared_ptr<Point> point = model.lock();
        if (!point) return;

        ImGui::SeparatorText("Point");

        ImGui::DragFloat3("Position", &point->GetPosition().x);
    }
};
