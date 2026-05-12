#include "element_interfaces.h"

#include "model/element/bezier_curve.h"

void ElementInterface::DrawTransformInterface(std::shared_ptr<Transform> transform) {
    if (!transform) return;

    ImGui::SeparatorText("Transform");

    auto pos = transform->GetPosition();
    ImGui::DragFloat3("Position", &pos.x);
    transform->SetPosition(pos);
    //TODO Command + csak a végén

    auto rot = transform->GetRotation();
    ImGui::DragFloat3("Rotattion", &rot.x);
    transform->SetRotation(rot);
    //TODO Command + csak a végén

    auto scale = transform->GetScale();
    ImGui::DragFloat3("Scale", &scale.x);
    transform->SetScale(scale);
    //TODO Command + csak a végén
}

void ElementInterface::DrawPointInterface(std::shared_ptr<Point> point) {
    if (!point) return;

    ImGui::SeparatorText("Point");

    auto pos = point->GetPosition();
    ImGui::DragFloat3("Location", &pos.x);
    point->SetPosition(pos);
    //TODO Command + csak a végén
}

void ElementInterface::DrawBezierNodeInterface(std::shared_ptr<BezierNode> node) {
    if (!node) return;

    ImGui::SeparatorText("Bezier Node");

    ImGui::Text("Set mode: ");
    ImGui::SameLine();
    if (ImGui::Button("Symmetric")) {
        node->SetMode(HandleMode::Symmetric);
        //TODO Command
    }
    ImGui::SameLine();
    if (ImGui::Button("Aligned")) {
        node->SetMode(HandleMode::Aligned);
        //TODO Command
    }
    ImGui::SameLine();
    if (ImGui::Button("Free")) {
        node->SetMode(HandleMode::Free);
        //TODO Command
    }
}

void ElementInterface::DrawBezierCurveInterface(std::shared_ptr<BezierCurve> curve) {
    if (!curve) return;

    ImGui::SeparatorText("Bezier Curve");

    if (ImGui::Button("Add Node to end")) {
        glm::vec3 lastPos;
        if (!curve->GetNodes().empty()) {
            auto lastPos = curve->GetNodes().back()->GetPoints()[0]->GetPosition();
        }
        curve->AddNode(std::make_shared<BezierNode>(lastPos + glm::vec3(1, 0, 0), HandleMode::Aligned));
        //TODO Command
    }
    if (ImGui::Button("Remove Last Node")) {
        curve->RemoveNodeLast();
        //TODO Command
    }
}
