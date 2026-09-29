#pragma once

#include <memory>
#include <vector>

#include "gl_engine/camera.hpp"
#include "gl_engine/material.hpp"
#include "gl_engine/mesh.hpp"
#include "../../model/element/bezier_curve.h"
#include "editor/view/curve_view.h"
#include "model/element/rotation_minimizing_frame.hpp"

class CurveRotationMinimizingFrameView {
private:
    std::unique_ptr<gl_engine::Mesh> mesh;
    std::unique_ptr<gl_engine::Material> material;

    std::shared_ptr<BezierCurve> curveModel;
    std::unique_ptr<RotationMinimizingFrame> rmf;

public:
    CurveRotationMinimizingFrameView(std::shared_ptr<BezierCurve> c);

    void Update(int resolution = 50, float length = 1.0f);

    void Draw();

private:
    void OnChanged() {
        Update(50, 1.0f);
    }
};
