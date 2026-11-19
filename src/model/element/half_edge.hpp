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
    HalfEdge(std::shared_ptr<ICurve> curve, float from = 0.0f, float to = 1.0f)
        : curve(curve), from(from), to(to) {
        curve->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged);
    }
    HalfEdge(std::shared_ptr<ICurve> curve, bool reversed)
        : curve(curve) {
        if (!reversed) /* normal */ {
            from = 0.0f;
            to = 1.0f;
        }
        else /* if reversed */ {
            from = 1.0f;
            to = 0.0f;
        }

        curve->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged);
    }

    ~HalfEdge() {

    }

    void InitializeAfterLoad() { curve.lock()->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged); }
    glm::vec3 EvaluatePosition(float t) const  {
        float newT = std::lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluatePosition(newT);
        else return glm::vec3(0.0f);
    }
    glm::vec3 EvaluateVelocity(float t) const {
        float newT = std::lerp(from, to, t);

        float scale = (to - from);
        if (auto s = curve.lock()) return scale * s->EvaluateVelocity(newT);
        else return glm::vec3(0.0f);
    }
    glm::vec3 EvaluateAcceleration(float t) const {
        float newT = std::lerp(from, to, t);

        float scale = (to - from);
        if (auto s = curve.lock()) return scale * scale * s->EvaluateAcceleration(newT);
        else return glm::vec3(0.0f);
    }
    float EvaluateCurveCurvature(float t) const {
        float newT = std::lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluateCurveCurvature(newT);
        else return 0.0f;
    }
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const {
        float newT = std::lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluateCurvePrincipalNormal(newT);
        else return glm::vec3(0.0f);
    }


    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(curve), CEREAL_NVP(from), CEREAL_NVP(to));
    }

private:
    void OnCurveChanged() {
        HalfEdgeChanged.Invoke();
    }
};