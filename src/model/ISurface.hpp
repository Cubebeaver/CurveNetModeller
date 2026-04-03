#pragma once

#include <glm/glm.hpp>

class ISurface : IElement {
public:
    virtual glm::vec3 Evaluate(float u, float v) const = 0;
    virtual glm::vec3 EvaluateNormal(float u, float v) const = 0;

    virtual ~ISurface() override = default;
};