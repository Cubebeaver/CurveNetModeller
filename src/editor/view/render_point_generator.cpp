#include "render_point_generator.h"

#include "model/element/i_Spline.hpp"

int RenderPointGenerator::GetSegmentCount(const ICurve& curveModel) {
    const auto* spline = dynamic_cast<const ISpline*>(&curveModel);
    if (spline) return spline->GetSegmentCount();
    else        return 1;
}

std::vector<glm::vec3> RenderPointGenerator::GetRenderPoints(const ICurve& curveModel, int resolution) {
    std::vector<glm::vec3> renderPoints;

    // int segments = GetSegmentCount();
    // if (segments == 0 && !Nodes.empty()) {
    //     renderPoints.push_back(Nodes[0]->GetCenterHandle()->GetPosition());
    //     return renderPoints;
    // }

    renderPoints.reserve(resolution);

    // for (int i = 0; i < segments; ++i) {
    //     int steps = (i == segments - 1) ? resolution : resolution - 1;
    //
    //     for (int step = 0; step <= steps; ++step) {
    //         float t = static_cast<float>(step) / static_cast<float>(resolution);
    //         renderPoints.push_back(EvaluateSegment(i, t));
    //     }
    // }

    for (int i = 0; i < resolution + 1; i++) {
        float t = static_cast<float>(i) / static_cast<float>(resolution);
        renderPoints.push_back(curveModel.EvaluatePosition(t));
    }

    return renderPoints;
}

//std::vector<glm::vec3> RenderPointGenerator::GetRenderNormals(const ICurve& curveModel, int resolution) {}
//std::vector<glm::vec3> RenderPointGenerator::GetRenderCameraNormals(const ICurve& curveModel, int resolution) {}
//std::vector<glm::vec3> RenderPointGenerator::GetRenderCurvatures(const ICurve& curveModel, int resolution) {}
