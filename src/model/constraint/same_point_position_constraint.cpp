#include "same_point_position_constraint.h"
#include "model/element/point.h"

SamePointPositionConstraint::SamePointPositionConstraint(std::shared_ptr<Point> self, std::shared_ptr<Point> target)
        : self(self), target(target) {
    self->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
    target->PointChanged += [&](auto delta) { if (!Verify()) Enforce(); };
}

void SamePointPositionConstraint::Enforce() {
    auto s = self.lock();
    auto t = target.lock();

    if (!s || !t) return;

    s->SetPosition(t->GetPosition());
}

bool SamePointPositionConstraint::Verify() const {
    auto s = self.lock();
    auto t = target.lock();

    if (!s || !t) return false;

    return glm::distance(s->GetPosition(), t->GetPosition()) < EditorConstants::MaxFloatError;
}