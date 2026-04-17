#pragma once
#include "i_interface.hpp"
#include "object_interface.hpp"
#include "editor/controller/curve_mesh_controller.h"

class CurveMeshInterface : public ObjectInterface {
private:
    std::weak_ptr<CurveMeshController> controller;

public:
    CurveMeshInterface(std::shared_ptr<CurveMeshController> controller) : ObjectInterface(controller->GetModel()), controller(controller) { }

    virtual void Draw() override {
        ObjectInterface::Draw();

        if (auto c = controller.lock()) {
            // ElementInterface::DrawCoonsSurfaceInterface(c->GetSelectedSurface().lock());
            // ElementInterface::DrawBezierCurveInterface(c->GetSelectedEdge().lock());
            ElementInterface::DrawBezierNodeInterface(c->GetSelectedNode().lock());
            ElementInterface::DrawPointInterface(c->GetSelectedPoint().lock());
        }
    };

    virtual ~CurveMeshInterface() override = default;
};
