#pragma once

#include "i_command.hpp"
#include "model/element/bezier_curve.h"


class RemoveNodeFromCurveCommand : public ICommand {
private:
    std::shared_ptr<BezierCurve> curve;
    std::shared_ptr<BezierNode> node;
    int idx;

public:
    RemoveNodeFromCurveCommand(std::shared_ptr<BezierCurve> curve, std::shared_ptr<BezierNode> node)
        : curve(curve), node(node) {
        idx = curve->IndexOf(node);
    }

    virtual void Execute() override {
        curve->RemoveNodeAt(idx);
    }

    virtual void Undo() override {
        curve->AddNodeAt(node, idx);
    }
};
