#pragma once

#include <glm/glm.hpp>

#include "i_Element.hpp"

class ICurve : public IElement {
public:
    virtual glm::vec3 EvaluatePosition(float t) const = 0;
    virtual float EvaluateCurveCurvature(float t) const = 0;
    virtual glm::vec3 EvaluateCurvePrincipalNormal(float t) const = 0;

    virtual ~ICurve() override = default;
};
