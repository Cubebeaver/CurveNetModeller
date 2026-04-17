#pragma once

#include "CONSTANTS.h"
#include "i_constraint.hpp"
#include "model/element/i_Curve.hpp"
#include "model/element/point.h"

class PointOnCurveConstraint : public IConstraint {
private:
    std::weak_ptr<Point> self;
    std::weak_ptr<ICurve> target;
    float parameter;

public:
    PointOnCurveConstraint(std::shared_ptr<Point> self, std::shared_ptr<ICurve> target, float parameter);

    virtual void Enforce() override;
    virtual bool Verify() const override;

    virtual ~PointOnCurveConstraint() = default;
};
