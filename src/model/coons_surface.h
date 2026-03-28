#pragma once

#include <complex>

#include "bezier_curve.h"
#include <glm/glm.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

class CoonsSurface {
public:
    std::weak_ptr<BezierCurve> c1;
    std::weak_ptr<BezierCurve> c2;
    std::weak_ptr<BezierCurve> d1;
    std::weak_ptr<BezierCurve> d2;

    Event<> CoonsSurfaceChanged;

    CoonsSurface() { }
    CoonsSurface(std::shared_ptr<BezierCurve> c1, std::shared_ptr<BezierCurve> c2,
                 std::shared_ptr<BezierCurve> d1, std::shared_ptr<BezierCurve> d2);

    glm::vec3 Evaluate(float u, float v) const;
    glm::vec3 EvaluateNormal(float u, float v) const;

    std::vector<glm::vec3> GetRenderPoints(int resolution) const;
    std::vector<glm::vec3> GetRenderNormals(int resolution) const;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(c1), CEREAL_NVP(c2), CEREAL_NVP(d1), CEREAL_NVP(d2));
    }
};
