#pragma once

#include <glm/glm.hpp>

#include "render_point_generator.h"
#include "../../model/element/bezier_curve.h"

#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/camera.hpp"
#include "gl_engine/shared_shaders.hpp"
using namespace gl_engine;

class CurveView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> material;

public:
    CurveView();

    void Update(const ICurve& curveModel, int resolution = 50);

    void Draw();
};
