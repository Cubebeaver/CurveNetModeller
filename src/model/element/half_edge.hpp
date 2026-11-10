#pragma once

#include "i_Curve.hpp"

#include <cereal/cereal.hpp>

class HalfEdge {
public:
    //TODO! VAGY WEAK???? de esélyes hogy shared, mert ha ez az iterátor le fogja cserélni az eredetit,
    //      akkor ennek kell majd életben tartania az eredetit.
    //      VAGY INKÁBB WEAK!!! mert a curve mesh az eredetit fogja eltárolni, max a felület akar majd
    std::weak_ptr<ICurve> curve;
    float from = 0;
    float to = 1;

    Event<> HalfEdgeChanged;

    HalfEdge() = default;
    HalfEdge(std::shared_ptr<ICurve> curve, float from = 0.0f, float to = 1.0f)
        : curve(curve), from(from), to(to) {
        curve->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged);
    }

    ~HalfEdge() {

    }

    void InitializeAfterLoad() { }
    glm::vec3 EvaluatePosition(float t) const  {
        float newT = Lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluatePosition(newT);
        else return glm::vec3(0.0f);
    }
    float EvaluateCurveCurvature(float t) const {
        float newT = Lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluateCurveCurvature(newT);
        else return 0.0f;
    }
    glm::vec3 EvaluateCurvePrincipalNormal(float t) const {
        float newT = Lerp(from, to, t);

        if (auto s = curve.lock()) return s->EvaluateCurvePrincipalNormal(newT);
        else return glm::vec3(0.0f);
    }


    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(curve), CEREAL_NVP(from), CEREAL_NVP(to));
    }
private:
    template<typename T>
    T static Lerp(T a, T b, T t) {
        return a + (b - a) * t;
    }

    void OnCurveChanged() {
        HalfEdgeChanged.Invoke();
    }
};