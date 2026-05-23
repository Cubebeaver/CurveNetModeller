#pragma once

#include <memory>

#include "i_command.hpp"
#include "model/object/curve_mesh.h"

class AddPointToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<Point> point;

public:
    AddPointToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<Point> point)
        : curveMesh(curveMesh), point(point) { }

    virtual void Execute() override {
        curveMesh->AddPoint(point);
    }

    virtual void Undo() override {
        curveMesh->RemovePoint(point);
    }
};
