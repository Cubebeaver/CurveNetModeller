#pragma once

#include <memory>

#include "i_command.hpp"
#include "model/object/curve_mesh.h"


class RemoveCoonsSurfaceFromCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<CoonsSurface> surface;

public:
    RemoveCoonsSurfaceFromCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<CoonsSurface> surface)
        : curveMesh(curveMesh), surface(surface) { }

    virtual void Execute() override {
        curveMesh->RemoveSurface(surface);
    }

    virtual void Undo() override {
        curveMesh->AddSurface(surface);
    }
};