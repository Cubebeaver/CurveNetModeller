#pragma once
#include "i_interface.hpp"
#include "object_interface.hpp"
#include "command/add_node_to_bezier_curve_command.hpp"
#include "command/command_history.hpp"
#include "command/composite_command.hpp"
#include "command/remove_bezier_curve_from_curve_mesh_command.hpp"
#include "command/remove_node_from_bezier_curve_command.hpp"
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
                        auto newNode = std::make_shared<BezierNode>(lastPos + glm::vec3(1, 0, 0), HandleMode::Aligned);

                        auto cmd = CompositeCommand::Create()
                        ->ExecuteAdd<AddNodeToBezierCurveCommand>(e, newNode, idx)
                        ->ExecuteAdd<AddPointToCurveMeshCommand>(c->GetModel(), newNode->GetCenterHandle())
                        ->ExecuteAdd<AddPointToCurveMeshCommand>(c->GetModel(), newNode->GetLeftHandle())
                        ->ExecuteAdd<AddPointToCurveMeshCommand>(c->GetModel(), newNode->GetRightHandle());
                        CommandHistory::Add(cmd);
                    }

                    if (ImGui::Button("Remove selected node")) {
                        auto cmd = CompositeCommand::Create();
                        cmd->ExecuteAdd<RemoveNodeFromBezierCurveCommand>(e, n);
                        cmd->ExecuteAdd<RemovePointToCurveMeshCommand>(c->GetModel(), n->GetCenterHandle());
                        cmd->ExecuteAdd<RemovePointToCurveMeshCommand>(c->GetModel(), n->GetLeftHandle());
                        cmd->ExecuteAdd<RemovePointToCurveMeshCommand>(c->GetModel(), n->GetRightHandle());

                        if (e->GetNodes().empty()) {
                            cmd->ExecuteAdd<RemoveBezierCurveFromCurveMeshCommand>(c->GetModel(), e);
                        }

                        CommandHistory::Add(cmd);
                    }
                }
            }

            if (auto e = c->GetActiveEdge().lock()) {
                if (ImGui::Button("Extrude selected edge")) {
                    c->ExtrudeSelectedEdge();
                    //TODO Command
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
                    //TODO Command
                }
            }

            ImGui::SeparatorText("Add");
            if (ImGui::Button("Add new curve")) {
                c->AddNewCurve();
            }

            if (ImGui::Button("Add new coons surface")) {
                c->AddNewCoonsSurface();
                //TODO Command
            }

            if (ImGui::Button("Add new bezier surface")) {
                c->AddNewBezierSurface();
                //TODO Command
            }

            ImGui::SeparatorText("Statistics");
            ImGui::Text("Points: %lld -- (selected: %lld)",   c->GetModel()->GetPoints().size(),   c->GetSelectedPoint().size());
            ImGui::Text("Edges: %lld -- (selected: %lld)",    c->GetModel()->GetEdges().size(),    c->GetSelectedEdge().size());
            ImGui::Text("Surfaces: %lld -- (selected: %lld)", c->GetModel()->GetSurfaces().size(), c->GetSelectedSurface().size());
            ImGui::Text("Selected Nodes: %lld",               c->GetSelectedNode().size());
        }
    };

    virtual ~CurveMeshInterface() override = default;
};
