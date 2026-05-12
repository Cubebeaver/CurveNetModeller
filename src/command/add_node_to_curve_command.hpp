#pragma once

#include "i_command.hpp"
#include "model/element/bezier_curve.h"


class AddNodeToCurveCommand : public ICommand {
private:
    std::shared_ptr<BezierCurve> curve;
    std::shared_ptr<BezierNode> node;
    int idx;

public:
    AddNodeToCurveCommand(std::shared_ptr<BezierCurve> curve, std::shared_ptr<BezierNode> node, int idx)
        : curve(curve), node(node), idx(idx) { }

    virtual void Execute() override {
        curve->AddNodeAt(node, idx);
    }

    virtual void Undo() override {
        curve->RemoveNodeAt(idx);
    }
};
