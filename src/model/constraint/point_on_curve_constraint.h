#pragma once

#include <memory>
#include "CONSTANTS.h"
#include "i_constraint.hpp"

class Point;
class ICurve;

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
