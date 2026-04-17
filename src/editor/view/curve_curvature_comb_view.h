#pragma once

#include <memory>
#include <vector>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "../../model/element/bezier_curve.h"
#include "editor/view/curve_view.h"

class CurveCurvatureCombView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> material;
    bool normalType = false;

public:
    CurveCurvatureCombView();

    void SetNormalType(bool normalType);

    void Update(const BezierCurve& curveModel, int resolution = 50, float length = 1.0f);

    void Draw();
};
