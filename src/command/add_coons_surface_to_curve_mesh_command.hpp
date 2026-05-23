#pragma once

#include <memory>

#include "i_command.hpp"
#include "model/object/curve_mesh.h"


class AddCoonsSurfaceToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<CoonsSurface> surface;

public:
    AddCoonsSurfaceToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<CoonsSurface> surface)
        : curveMesh(curveMesh), surface(surface) { }

    virtual void Execute() override {
        curveMesh->AddSurface(surface);
    }

    virtual void Undo() override {
        curveMesh->RemoveSurface(surface);
    }
};