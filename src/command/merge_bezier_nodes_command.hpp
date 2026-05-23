#pragma once

#include "inverse_command.hpp"
#include "i_command.hpp"
#include "model/element/bezier_node.h"


class MergeBezierNodesCommand : public ICommand {
private:
    std::shared_ptr<BezierNode> nodeToReplace;
    std::shared_ptr<BezierNode> targetNode;
    std::shared_ptr<Point> pointToReplace;
    std::shared_ptr<Point> targetPoint;

public:
    MergeBezierNodesCommand(std::shared_ptr<BezierNode> nodeToReplace, std::shared_ptr<BezierNode> targetNode)
        : nodeToReplace(nodeToReplace), targetNode(targetNode) {
        pointToReplace = nodeToReplace->GetCenterHandle();
        targetPoint    = targetNode->GetCenterHandle();
    }

    virtual void Execute() override {
        nodeToReplace->SetCenterHandle(targetPoint);
    }

    virtual void Undo() override {
        nodeToReplace->SetCenterHandle(pointToReplace);
    }
};