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
            ElementInterface::DrawPointInterface(c->GetActivePoint().lock());
            ElementInterface::DrawBezierNodeInterface(c->GetActiveNode().lock());
            ElementInterface::DrawBezierCurveInterface(c->GetActiveEdge().lock());

            if (auto e = c->GetActiveEdge().lock()) {
                if (auto n = c->GetActiveNode().lock()) {
                    if (ImGui::Button("Add node after selection")) {
                        int idx = e->IndexOf(n);
                        auto lastPos = n->GetPoints()[0]->GetPosition();
                        e->AddNodeAt(std::make_shared<BezierNode>(lastPos + glm::vec3(1, 0, 0), HandleMode::Aligned), idx);
                    }

                    if (ImGui::Button("Remove selected node")) {
                        e->RemoveNode(n);
                        if (e->GetNodes().empty()) {
                            controller.lock()->GetModel()->RemoveEdge(e);
                        }
                    }
                }
            }

            if (auto e = c->GetActiveEdge().lock()) {
                if (ImGui::Button("Extrude selected edge")) {
                    c->ExtrudeSelectedEdge();
                }
            }

            if (c->GetSelectedNode().size() >= 2) {
                if (ImGui::Button("Merge selected nodes")) {
                    c->MergeNodes();
                }
            }

            if (!c->GetSelectedNode().empty()) {
                if (ImGui::Button("Split selected nodes")) {
                    c->SplitNodes();
                }
            }

            if (c->GetSelectedEdge().size() == 4) {
                if (ImGui::Button("Fill with Coons surface")) {
                    c->FillCoons();
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
