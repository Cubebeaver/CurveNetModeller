#pragma once

#include "model/element/bezier_node.h"

#include "gl_engine/mesh.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/camera.hpp"
#include "gl_engine/shared_shaders.hpp"
using namespace gl_engine;

class BezierNodeView {
private:
    std::unique_ptr<Mesh> mesh;
    std::unique_ptr<Material> lineMat;
    std::unique_ptr<Material> pointMat;
    std::unique_ptr<Material> selectedMat;

public:
    BezierNodeView();

    void Update(const BezierNode& nodeModel);

    void Draw(BezierHandleType selected = BezierHandleType::None);
};
