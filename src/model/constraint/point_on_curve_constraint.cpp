#include "point_on_curve_constraint.h"
#include "model/element/point.h"
#include "model/element/i_Curve.hpp"

PointOnCurveConstraint::PointOnCurveConstraint(std::shared_ptr<Point> self, std::shared_ptr<ICurve> target, float parameter)
: self(self), target(target), parameter(parameter) {
    if (parameter < 0 || parameter > 1) std::cout << "[!] t must be between 0 and 1";

    self->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
    target->CurveChanged += [&]() { if (!Verify()) Enforce(); };
}

void PointOnCurveConstraint::Enforce() {
    auto s = self.lock();
    auto t = target.lock();

    if (!s || !t) return;

    auto tpos = t->EvaluatePosition(parameter);
    s->SetPosition(tpos);
}
bool PointOnCurveConstraint::Verify() const {
    auto s = self.lock();
    auto t = target.lock();

    //TODO vagy lehet ide inkább true kéne??
    //     (mert ha if (!Verify()) Enforce() van akkor már ne próbáljunk enforceolni)
    if (!s || !t) return false;

    auto tpos = t->EvaluatePosition(parameter);
    return glm::distance(s->GetPosition(), tpos) < EditorConstants::MaxFloatError;
}