#pragma once
#include "i_interface.hpp"
#include "object_interface.hpp"
#include "editor/controller/curve_mesh_controller.h"

class CurveMeshInterface : public ObjectInterface {
private:
    std::weak_ptr<CurveMeshController> controller;

public:
    CurveMeshInterface(std::shared_ptr<CurveMeshController> controller) : ObjectInterface(controller->GetModel()), controller(controller) { }

    virtual void Draw() override {
        ObjectInterface::Draw();

        if (auto c = controller.lock()) {
            // ElementInterface::DrawCoonsSurfaceInterface(c->GetSelectedSurface().lock());
            ElementInterface::DrawPointInterface(c->GetSelectedPoint().lock());
            ElementInterface::DrawBezierNodeInterface(c->GetSelectedNode().lock());
            ElementInterface::DrawBezierCurveInterface(c->GetSelectedEdge().lock());

            if (auto e = c->GetSelectedEdge().lock()) {
                if (auto n = c->GetSelectedNode().lock()) {
                    if (ImGui::Button("Add node after selection")) {
                        int idx = e->IndexOf(n);
                        auto lastPos = n->GetPoints()[0]->GetPosition();
                        e->AddNodeAt(std::make_shared<BezierNode>(lastPos + glm::vec3(1, 0, 0), HandleMode::Aligned), idx);
                    }

                    if (ImGui::Button("Remove selected node")) {
                        e->RemoveNode(n);
                    }
                }
            }

            ImGui::SeparatorText("Add");
            if (ImGui::Button("Add new curve")) {
                c->AddNewCurve();
            }

            if (ImGui::Button("Add new surface")) {
                c->AddNewSurface();
            }

        }
    };

    virtual ~CurveMeshInterface() override = default;
};
