#pragma once

#include "i_command.hpp"
#include "model/object/curve_mesh.h"


class AddBezierCurveToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<BezierCurve> curve;

public:
    AddBezierCurveToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<BezierCurve> curve)
        : curveMesh(curveMesh), curve(curve) { }

    virtual void Execute() override {
        curveMesh->AddEdge(curve);
    }

    virtual void Undo() override {
        curveMesh->RemoveEdge(curve);
    }
};

//using RemoveCurveFromCurveMesh = InverseCommand<AddCurveToCurveMesh>;