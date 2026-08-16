#pragma once

#include <glm/glm.hpp>

#include <cereal/types/polymorphic.hpp>

class ISurface : public IElement {
public:
    Event<> CoonsSurfaceChanged;

    virtual glm::vec3 Evaluate(float u, float v) const = 0;

    /**
     * Returns the normal of the surface at a given parameter.
     * This default implementation calculates the normal in a numeric fashion (with 0.01 delta),
     * so it is not precise and derived classes should override this.
     * @param u The u parameter
     * @param v The v parameter
     * @return The normal at the (u, v) parameter
     */
    virtual glm::vec3 EvaluateNormal(float u, float v) const {
        const float dx = 0.01f;

        glm::vec3 c = Evaluate(u, v);
        glm::vec3 du = Evaluate(u + dx, v);
        glm::vec3 dv = Evaluate(u, v + dx);

        glm::vec3 du_dx = du - c;
        glm::vec3 dv_dx = dv - c;

        glm::vec3 normal = glm::normalize(glm::cross(dv_dx, du_dx));
        return normal;
    }

    virtual ~ISurface() override = default;
};