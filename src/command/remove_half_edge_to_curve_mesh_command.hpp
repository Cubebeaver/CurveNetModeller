#pragma once

#include "i_command.hpp"
#include "model/object/curve_mesh.h"

class RemoveHalfEdgeToCurveMeshCommand : public ICommand {
private:
    std::shared_ptr<CurveMesh> curveMesh;
    std::shared_ptr<HalfEdge> halfEdge;

public:
    RemoveHalfEdgeToCurveMeshCommand(std::shared_ptr<CurveMesh> curveMesh, std::shared_ptr<HalfEdge> halfEdge)
        : curveMesh(curveMesh), halfEdge(halfEdge) { }

    virtual void Execute() override {
        curveMesh->RemoveHalfEdge(halfEdge);
    }

    virtual void Undo() override {
        curveMesh->AddHalfEdge(halfEdge);
    }
};