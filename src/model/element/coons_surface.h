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

    CoonsSurface() { }
    CoonsSurface(std::shared_ptr<BezierCurve> c1, std::shared_ptr<BezierCurve> c2,
                 std::shared_ptr<BezierCurve> d1, std::shared_ptr<BezierCurve> d2);

    glm::vec3 Evaluate(float u, float v) const override;

    virtual ~CoonsSurface() override;

    void InitializeAfterLoad() override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(c1), CEREAL_NVP(c2), CEREAL_NVP(d1), CEREAL_NVP(d2));
    }

private:
    void OnCurveChanged();
};
