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
    SamePointPositionConstraint(std::shared_ptr<Point> self, std::shared_ptr<Point> target)
        : self(self), target(target) {
        self->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
        target->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
    }

    virtual void Enforce() override {
        auto s = self.lock();
        auto t = target.lock();

        if (!s || !t) return;

        s->SetPosition(t->GetPosition());
    }

    virtual bool Verify() const override {
        auto s = self.lock();
        auto t = target.lock();

        if (!s || !t) return false;

        return glm::distance(s->GetPosition(), t->GetPosition()) < MAX_FLOAT_ERROR;
    }

    virtual ~SamePointPositionConstraint() = default;
};
