#pragma once
#include <memory>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>

#include "CONSTANTS.h"
#include "i_constraint.hpp"

class Point;

class SamePointPositionConstraint : public IConstraint {
private:
    std::weak_ptr<Point> self;
    std::weak_ptr<Point> target;

public:
    SamePointPositionConstraint(std::shared_ptr<Point> self, std::shared_ptr<Point> target);

    virtual void Enforce() override;
    virtual bool Verify() const override;

    virtual ~SamePointPositionConstraint();

private:
    void OnPointChanged(const glm::vec3 offset);
};
