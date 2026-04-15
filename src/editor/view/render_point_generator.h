#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "model/element/i_Curve.hpp"

class RenderPointGenerator {
public:
    static int GetSegmentCount(const ICurve& curveModel);

    static std::vector<glm::vec3> GetRenderPoints(const ICurve& curveModel, int resolution);
    static std::vector<glm::vec3> GetRenderNormals(const ICurve& curveModel, int resolution);
    static std::vector<glm::vec3> GetRenderCameraNormals(const ICurve& curveModel, int resolution);
    static std::vector<glm::vec3> GetRenderCurvatures(const ICurve& curveModel, int resolution);
};
