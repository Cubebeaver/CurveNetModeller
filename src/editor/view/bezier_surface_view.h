#pragma once
#include <memory>

#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "model/element/bezier_surface.h"


class BezierSurfaceView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> lineMat;
    std::unique_ptr<gl_engine::Material> pointMat;
    std::unique_ptr<gl_engine::Material> selectedMat;

    int u = 0, v = 0;

public:
    BezierSurfaceView();

    void Update(const BezierSurface& surfaceModel);

    void Draw();
};
