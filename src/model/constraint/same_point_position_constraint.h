#pragma once
#include <memory>
#include <glm/detail/func_geometric.inl>

#include "CONSTANTS.h"
#include "i_constraint.hpp"
#include "model/element/point.h"

class SamePointPositionConstraint : public IConstraint {
private:
    std::weak_ptr<Point> self;
    std::weak_ptr<Point> target;

public:
    SamePointPositionConstraint(std::shared_ptr<Point> self, std::shared_ptr<Point> target);

    virtual void Enforce() override;
    virtual bool Verify() const override;

    virtual ~SamePointPositionConstraint() = default;
};
