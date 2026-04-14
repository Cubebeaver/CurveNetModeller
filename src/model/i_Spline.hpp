#pragma once

#include <glm/glm.hpp>

#include "i_Curve.hpp"

class ISpline : public ICurve {
public:
    virtual glm::vec3 EvaluateSegment(int segmentIndex, float t) const = 0;
    virtual float EvaluateSegmentCurvature(int segmentIndex, float t) const = 0;
    virtual glm::vec3 EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const = 0;

    virtual ~ISpline() override = default;
};

