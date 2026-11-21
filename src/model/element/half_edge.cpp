#include "half_edge.h"

HalfEdge::HalfEdge(std::shared_ptr<ICurve> curve, float from, float to): curve(curve), from(from), to(to) {
    curve->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged);
}

HalfEdge::HalfEdge(std::shared_ptr<ICurve> curve, bool reversed): curve(curve) {
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

inline HalfEdge::~HalfEdge() {
    if (auto s = curve.lock()) s->CurveChanged.RemoveListener(this, &HalfEdge::OnCurveChanged);
}

inline void HalfEdge::InitializeAfterLoad() {
    curve.lock()->CurveChanged.AddListener(this, &HalfEdge::OnCurveChanged);
}

inline glm::vec3 HalfEdge::EvaluatePosition(float t) const {
    float newT = std::lerp(from, to, t);

    if (auto s = curve.lock()) return s->EvaluatePosition(newT);
    else return glm::vec3(0.0f);
}

inline glm::vec3 HalfEdge::EvaluateVelocity(float t) const {
    float newT = std::lerp(from, to, t);

    float scale = (to - from);
    if (auto s = curve.lock()) return scale * s->EvaluateVelocity(newT);
    else return glm::vec3(0.0f);
}

inline glm::vec3 HalfEdge::EvaluateAcceleration(float t) const {
    float newT = std::lerp(from, to, t);

    float scale = (to - from);
    if (auto s = curve.lock()) return scale * scale * s->EvaluateAcceleration(newT);
    else return glm::vec3(0.0f);
}

inline float HalfEdge::EvaluateCurveCurvature(float t) const {
    float newT = std::lerp(from, to, t);

    if (auto s = curve.lock()) return s->EvaluateCurveCurvature(newT);
    else return 0.0f;
}

inline glm::vec3 HalfEdge::EvaluateCurvePrincipalNormal(float t) const {
    float newT = std::lerp(from, to, t);

    if (auto s = curve.lock()) return s->EvaluateCurvePrincipalNormal(newT);
    else return glm::vec3(0.0f);
}
