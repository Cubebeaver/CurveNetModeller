#pragma once

#include "bezier_curve.h"
#include <glm/glm.hpp>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "i_Surface.hpp"


class CoonsSurface : public ISurface {
public:
    std::shared_ptr<BezierCurve> c1;
    std::shared_ptr<BezierCurve> c2;
    std::shared_ptr<BezierCurve> d1;
    std::shared_ptr<BezierCurve> d2;

    Event<> CoonsSurfaceChanged;

    CoonsSurface() { }
    CoonsSurface(std::shared_ptr<BezierCurve> c1, std::shared_ptr<BezierCurve> c2,
                 std::shared_ptr<BezierCurve> d1, std::shared_ptr<BezierCurve> d2);

    glm::vec3 Evaluate(float u, float v) const override;
    glm::vec3 EvaluateNormal(float u, float v) const override;

    std::vector<glm::vec3> GetRenderPoints(int resolution) const;
    std::vector<glm::vec3> GetRenderNormals(int resolution) const;

    virtual ~CoonsSurface() = default;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(c1), CEREAL_NVP(c2), CEREAL_NVP(d1), CEREAL_NVP(d2));
    }
};
