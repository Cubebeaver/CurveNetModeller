#pragma once

#include "i_command.hpp"
#include "model/element/bezier_curve.h"
#include "model/object/curve_mesh.h"


class RemoveCurveFromCurveMesh : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<BezierCurve> curve;

public:
    RemoveCurveFromCurveMesh(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<BezierCurve> curve)
        : curveMesh(curveMesh), curve(curve) { }

    virtual void Execute() override {
        curveMesh->RemoveEdge(curve);
    }

    virtual void Undo() override {
        curveMesh->AddEdge(curve);
    }
};
