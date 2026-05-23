#pragma once

#include "inverse_command.hpp"
#include "i_command.hpp"
#include "model/element/bezier_node.h"


class SplitBezierNodesCommand : public ICommand {
private:
    struct ResoreNode {
        std::shared_ptr<BezierNode> node;
        std::shared_ptr<Point> originalPoint;
    };

    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<BezierNode> targetNode;
    std::vector<ResoreNode> nodesToBeRestored;

public:
    SplitBezierNodesCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<BezierNode> targetNode)
        : curveMesh(curveMesh), targetNode(targetNode) { }

    virtual void Execute() override {
        auto ps = targetNode->GetCenterHandle();

        for (auto c : curveMesh->GetEdges()) {
            for (auto cn : c->GetNodes()) {
                auto cps = cn->GetCenterHandle();
                if (ps == cps && targetNode != cn) {
                    nodesToBeRestored.clear();
                    nodesToBeRestored.push_back({ cn, cps });
                    auto splitPoint = std::make_shared<Point>(ps->GetPosition());
                    curveMesh->AddPoint(splitPoint);
                    cn->SetCenterHandle(splitPoint);
                }
            }
        }
    }

    virtual void Undo() override {
        for (auto& n : nodesToBeRestored) {
            curveMesh->RemovePoint(n.node->GetCenterHandle());
            n.node->SetCenterHandle(n.originalPoint);

        }
    }
};