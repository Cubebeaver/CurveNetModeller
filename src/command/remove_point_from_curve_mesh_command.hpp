#pragma once

#include <memory>

#include "i_command.hpp"
#include "model/object/curve_mesh.h"

class RemovePointToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<Point> point;

public:
    RemovePointToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<Point> point)
        : curveMesh(curveMesh), point(point) { }

    virtual void Execute() override {
        curveMesh->RemovePoint(point);
    }

    virtual void Undo() override {
        curveMesh->AddPoint(point);
    }
};
