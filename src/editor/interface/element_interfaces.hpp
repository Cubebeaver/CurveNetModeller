#pragma once
#include <imgui.h>
#include <memory>

#include "model/element/bezier_node.h"
#include "model/element/point.h"
#include "model/object/transform.h"

class ElementInterface {
public:
    static void DrawTransformInterface(std::shared_ptr<Transform> transform) {
        if (!transform) return;

        ImGui::SeparatorText("Transform");

        auto pos = transform->GetPosition();
        ImGui::DragFloat3("Position", &pos.x);
        transform->SetPosition(pos);

        auto rot = transform->GetRotation();
        ImGui::DragFloat3("Rotattion", &rot.x);
        transform->SetRotation(rot);

        auto scale = transform->GetScale();
        ImGui::DragFloat3("Scale", &scale.x);
        transform->SetScale(scale);
    }

    static void DrawPointInterface(std::shared_ptr<Point> point) {
        if (!point) return;

        ImGui::SeparatorText("Point");

        auto pos = point->GetPosition();
        ImGui::DragFloat3("Location", &pos.x);
        point->SetPosition(pos);
    }

    static void DrawBezierNodeInterface(std::shared_ptr<BezierNode> node) {
        if (!node) return;

        ImGui::SeparatorText("Bezier Node");

        ImGui::Text("Set mode: ");
        ImGui::SameLine();
        if (ImGui::Button("Symmetric")) {
            node->SetMode(HandleMode::Symmetric);
        }
        ImGui::SameLine();
        if (ImGui::Button("Aligned")) {
            node->SetMode(HandleMode::Aligned);
        }
        ImGui::SameLine();
        if (ImGui::Button("Free")) {
            node->SetMode(HandleMode::Free);
        }
    }
};
