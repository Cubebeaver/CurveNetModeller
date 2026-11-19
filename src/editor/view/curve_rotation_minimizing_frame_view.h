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
    std::unique_ptr<gl_engine::Material> materialR;
    std::unique_ptr<gl_engine::Material> materialT;
    std::unique_ptr<gl_engine::Material> materialS;

    std::shared_ptr<ICurve> curveModel;
    std::unique_ptr<RotationMinimizingFrame> rmf;

    bool drawSpace = true;
    int lastResolution = 10;

public:
    CurveRotationMinimizingFrameView(std::shared_ptr<ICurve> c, bool drawSpace = true);

    void Update(int resolution = 10, float length = 0.1f);

    void Draw();

    ~CurveRotationMinimizingFrameView() {
        curveModel->CurveChanged.RemoveListener(this, &CurveRotationMinimizingFrameView::OnChanged);
    }

private:
    void OnChanged() {
        Update();
    }
};
