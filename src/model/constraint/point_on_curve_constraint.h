#pragma once

#include <memory>
#include <glm/vec3.hpp>

#include "CONSTANTS.h"
#include "i_constraint.hpp"
#include "model/element/bezier_curve.h"
#include "model/element/bezier_curve.h"

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

    virtual ~PointOnCurveConstraint() override;

private:
    void OnPointChanged(const glm::vec3 offset);
    void OnCurveChanged();
};
