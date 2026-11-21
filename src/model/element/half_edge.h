#pragma once

#include "i_Curve.hpp"
#include "util/math_helper.hpp"

#include <cereal/cereal.hpp>

class HalfEdge {
public:
    //TODO! VAGY WEAK???? de esélyes hogy shared, mert ha ez az iterátor le fogja cserélni az eredetit,
    //      akkor ennek kell majd életben tartania az eredetit.
    //      VAGY INKÁBB WEAK!!! mert a curve mesh az eredetit fogja eltárolni, max a felület akar majd
    std::weak_ptr<ICurve> curve;
    float from = 0.0f;
    float to = 1.0f;

    Event<> HalfEdgeChanged;

    HalfEdge() = default;
    HalfEdge(std::shared_ptr<ICurve> curve, float from = 0.0f, float to = 1.0f);
    HalfEdge(std::shared_ptr<ICurve> curve, bool reversed);

    void InitializeAfterLoad();

    ~HalfEdge();

    glm::vec3 EvaluatePosition(float t) const;
    glm::vec3 EvaluateVelocity(float t) const;
    glm::vec3 EvaluateAcceleration(float t) const;
    float EvaluateCurveCurvature(float t) const;
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const;


    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(curve), CEREAL_NVP(from), CEREAL_NVP(to));
    }

private:
    void OnCurveChanged() {
        HalfEdgeChanged.Invoke();
    }
};