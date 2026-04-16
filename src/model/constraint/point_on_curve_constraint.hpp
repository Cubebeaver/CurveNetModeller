#pragma once

#include "CONSTANTS.h"
#include "i_constraint.hpp"
#include "model/element/i_Curve.hpp"
#include "model/element/point.h"

class PointOnCurveConstraint : public IConstraint {
private:
    std::weak_ptr<Point> self;
    std::weak_ptr<ICurve> target;
    float t;

public:
    PointOnCurveConstraint(std::shared_ptr<Point> self, std::shared_ptr<ICurve> target, float t)
        : self(self), target(target), t(t) {
        if (t < 0 || t > 1) std::cout << "[!] t must be between 0 and 1";

        self->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
        target->CurveChanged += [&]() { if (!Verify()) Enforce(); };
    }

    virtual void Enforce() override {
        auto s = self.lock();
        auto t = target.lock();

        if (!s || !t) return;

        auto tpos = t->EvaluatePosition(t);
        s->SetPosition(tpos);
    }
    virtual bool Verify() const override {
        auto s = self.lock();
        auto t = target.lock();

        //TODO vagy lehet ide inkább true kéne??
        //     (mert ha if (!Verify()) Enforce() van akkor már ne próbáljunk enforceolni)
        if (!s || !t) return false;

        auto tpos = t->EvaluatePosition(t);
        return glm::distance(s->GetPosition(), tpos) < MAX_FLOAT_ERROR;
    }

    virtual ~PointOnCurveConstraint() = default;
};
