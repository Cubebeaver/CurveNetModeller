#pragma once

#include <imgui.h>
#include <memory>

#include "model/element/bezier_curve.h"
#include "model/element/bezier_node.h"
#include "model/element/point.h"
#include "model/object/transform.h"

/* static */ class ElementInterface {
public:
    static void DrawTransformInterface(std::shared_ptr<Transform> transform);
    static void DrawPointInterface(std::shared_ptr<Point> point);
    static void DrawBezierNodeInterface(std::shared_ptr<BezierNode> node);
    static void DrawBezierCurveInterface(std::shared_ptr<BezierCurve> node);
};
