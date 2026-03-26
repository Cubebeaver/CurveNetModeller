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
    CoonsSurface(
        std::shared_ptr<BezierCurve> c1, std::shared_ptr<BezierCurve> c2,
        std::shared_ptr<BezierCurve> d1, std::shared_ptr<BezierCurve> d2
    ) : c1(c1), c2(c2), d1(d1), d2(d2) {
        this->c1.lock()->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
        this->c2.lock()->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
        this->d1.lock()->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
        this->d2.lock()->BezierCurveChanged += [&](){ CoonsSurfaceChanged.Invoke(); };
    }

    glm::vec3 Evaluate(float u, float v) const {
        const auto& S_u0 = c1.lock()->EvaluateCurve(u);
        const auto& S_u1 = c2.lock()->EvaluateCurve(u);
        const auto& S_0v = d1.lock()->EvaluateCurve(v);
        const auto& S_1v = d2.lock()->EvaluateCurve(v);

        const auto& S1_uv = (1 - v) * S_u0 + v * S_u1;
        const auto& S2_uv = (1 - u) * S_0v + u * S_1v;

        const auto& S_00 = c1.lock()->EvaluateCurve(0);
        const auto& S_10 = c1.lock()->EvaluateCurve(1);
        const auto& S_01 = c2.lock()->EvaluateCurve(0);
        const auto& S_11 = c2.lock()->EvaluateCurve(1);

        const auto& S12_uv = (1 - v) * ((1 - u) * S_00 + u * S_10)
                                 + v * ((1 - u) * S_01 + u * S_11);

        return S1_uv + S2_uv - S12_uv;
    }

    std::vector<glm::vec3> GetRenderPoints(int resolution) const {
        std::vector<glm::vec3> surfacePoints;
        surfacePoints.reserve(resolution * resolution);
        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                surfacePoints.push_back(Evaluate((float)i / (resolution - 1), (float)j / (resolution - 1)));
            }
        }
        return surfacePoints;
    }

    std::vector<glm::vec3> GetRenderNormals(int resolution) const {
        const float dx = 0.01f;

        std::vector<glm::vec3> surfaceNormals;
        surfaceNormals.reserve(resolution * resolution);
        for (int i = 0; i < resolution; i++) {
            for (int j = 0; j < resolution; j++) {
                glm::vec3 c = Evaluate((float)i / (resolution - 1), (float)j / (resolution - 1));
                glm::vec3 u = Evaluate((float)i / (resolution - 1) + dx, (float)j / (resolution - 1));
                glm::vec3 v = Evaluate((float)i / (resolution - 1), (float)j / (resolution - 1) + dx);

                glm::vec3 du_dx = u - c;
                glm::vec3 dv_dx = v - c;

                glm::vec3 normal = glm::normalize(glm::cross(dv_dx, du_dx));
                surfaceNormals.push_back(normal);
            }
        }
        return surfaceNormals;
    }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(c1), CEREAL_NVP(c2), CEREAL_NVP(d1), CEREAL_NVP(d2));
    }
};
