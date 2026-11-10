#pragma once

#include "i_command.hpp"
#include "model/object/curve_mesh.h"

class AddHalfEdgeToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<HalfEdge> halfEdge;

public:
    AddHalfEdgeToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<HalfEdge> halfEdge)
        : curveMesh(curveMesh), halfEdge(halfEdge) { }

    virtual void Execute() override {
        curveMesh->AddHalfEdge(halfEdge);
    }

    virtual void Undo() override {
        curveMesh->RemoveHalfEdge(halfEdge);
    }
};