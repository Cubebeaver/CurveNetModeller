#pragma once

#include <glm/glm.hpp>

#include "IElement.hpp"

class ICurve : public IElement {
public:
    virtual glm::vec3 EvaluateCurve(float t) const = 0;
    virtual float EvaluateCurveCurvature(float t) const = 0;
    virtual glm::vec3 EvaluateCurvePrincipalNormal(float t) const = 0;

    virtual ~ICurve() override = default;
};
