#pragma once

#include <glm/glm.hpp>

#include "i_Element.hpp"
#include "util/event.hpp"

class ICurve : public IElement {
public:
    Event<> CurveChanged;

    virtual glm::vec3 EvaluatePosition(float t) const = 0;
    virtual glm::vec3 EvaluateVelocity(float t) const = 0;
    virtual glm::vec3 EvaluateAcceleration(float t) const = 0;
    virtual float EvaluateCurveCurvature(float t) const = 0;
    virtual glm::vec3 EvaluateCurvePrincipalNormal(float t) const = 0;

    virtual ~ICurve() override = default;
};
