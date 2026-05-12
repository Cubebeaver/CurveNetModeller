#pragma once

#include "i_command.hpp"
#include "model/object/curve_mesh.h"


class AddCurveToCurveMesh : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<BezierCurve> curve;

public:
    AddCurveToCurveMesh(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<BezierCurve> curve)
        : curveMesh(curveMesh), curve(curve) { }

    virtual void Execute() override {
        curveMesh->AddEdge(curve);
    }

    virtual bool Undo() override {
        curveMesh->RemoveEdge(curve);
    }
};

//using RemoveCurveFromCurveMesh = InverseCommand<AddCurveToCurveMesh>;