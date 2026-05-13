#pragma once

#include "inverse_command.hpp"
#include "i_command.hpp"
#include "model/element/bezier_node.h"


class SplitNodesCommand : public ICommand {
private:
    struct ResoreNode {
        std::shared_ptr<BezierNode> node;
        std::shared_ptr<Point> originalPoint;
    };

    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<BezierNode> targetNode;
    std::vector<ResoreNode> nodesToBeRestored;

public:
    SplitNodesCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<BezierNode> targetNode)
        : curveMesh(curveMesh), targetNode(targetNode) { }

    virtual void Execute() override {
        auto ps = targetNode->GetCenterHandle();

        for (auto c : curveMesh->GetEdges()) {
            for (auto cn : c->GetNodes()) {
                auto cps = cn->GetCenterHandle();
                if (ps == cps && targetNode != cn) {
                    nodesToBeRestored.push_back({ cn, cps });
                    cn->SetCenterHandle(std::make_shared<Point>(ps->GetPosition()));
                }
            }
        }
    }

    virtual void Undo() override {
        for (auto& n : nodesToBeRestored) {
            n.node->SetCenterHandle(n.originalPoint);
        }
    }
};