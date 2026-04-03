#pragma once
#include <imgui.h>
#include <memory>

#include "IInterface.hpp"
#include "model/point.h"

class PointInterface : public IInterface {
private:
    std::weak_ptr<Point> modelPoint;

public:
    void Draw() override {
        std::shared_ptr<Point> point = modelPoint.lock();
        if (!point) return;

        float p[3];
        p[0] = point->GetPosition().x;
        p[1] = point->GetPosition().y;
        p[2] = point->GetPosition().z;
        ImGui::DragFloat3("Position", p);
        point->SetPosition(glm::vec3(p[0], p[1], p[2]));
    }
};
