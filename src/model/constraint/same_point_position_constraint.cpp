#include "same_point_position_constraint.h"
#include "model/element/point.h"

SamePointPositionConstraint::SamePointPositionConstraint(std::shared_ptr<Point> self, std::shared_ptr<Point> target)
        : self(self), target(target) {
    self->PointChanged.AddListener(this, &SamePointPositionConstraint::OnPointChanged);
    target->PointChanged.AddListener(this, &SamePointPositionConstraint::OnPointChanged);
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

SamePointPositionConstraint::~SamePointPositionConstraint() {
    if (auto s = self.lock())   s->PointChanged.RemoveListener(this, &SamePointPositionConstraint::OnPointChanged);
    if (auto t = target.lock()) t->PointChanged.RemoveListener(this, &SamePointPositionConstraint::OnPointChanged);
}

void SamePointPositionConstraint::OnPointChanged(const glm::vec3 offset) {
    if (!Verify()) Enforce();
}
