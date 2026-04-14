#include "model/bezier_node.h"



BezierNode::BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode) 
    : CenterHandle(std::make_shared<Point>(position)), LeftHandle(std::make_shared<Point>(leftHandle)), RightHandle(std::make_shared<Point>(rightHandle)), Mode(mode) {
    CenterHandle->PointChanged += [&](glm::vec3 delta){ LeftHandle->GetPosition() += delta; RightHandle->GetPosition() += delta; BezierNodeChanged.Invoke(); };
    LeftHandle->PointChanged += [&](glm::vec3 delta){ EnforceMode(true); BezierNodeChanged.Invoke(); };
    RightHandle->PointChanged += [&](glm::vec3 delta){ EnforceMode(false); BezierNodeChanged.Invoke(); };

    EnforceMode(true);
    BezierNodeChanged.Invoke();
}

BezierNode::BezierNode(HandleMode mode) 
    : BezierNode(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), mode) { }

BezierNode::BezierNode(glm::vec3 position, HandleMode mode)
    : BezierNode(position, position + glm::vec3(-1, 0, 0), position + glm::vec3(1, 0, 0), mode) { }

BezierHandleType BezierNode::GetHandleType(std::weak_ptr<Point> point) const {
    auto p = point.lock();
    if (!p) return BezierHandleType::None;

    if (p == CenterHandle) return BezierHandleType::Center;
    if (p == LeftHandle) return BezierHandleType::Left;
    if (p == RightHandle) return BezierHandleType::Right;
    return BezierHandleType::None;
}

void BezierNode::SetPosition(const glm::vec3& newPos) {
    glm::vec3 delta = newPos - CenterHandle->GetPosition();
    CenterHandle->GetPosition() = newPos;
    LeftHandle->GetPosition() += delta;
    RightHandle->GetPosition() += delta;

    BezierNodeChanged.Invoke();
}
void BezierNode::SetPosition(const std::shared_ptr<Point> newPoint) {
    CenterHandle = newPoint;
    glm::vec3 delta = newPoint->GetPosition() - CenterHandle->GetPosition();
    LeftHandle->GetPosition() += delta;
    RightHandle->GetPosition() += delta;

    BezierNodeChanged.Invoke();
}


void BezierNode::SetLeftHandle(const glm::vec3& newPos) {
    LeftHandle->GetPosition() = newPos;
    EnforceMode(true);
    BezierNodeChanged.Invoke();
}
void BezierNode::SetLeftHandle(const std::shared_ptr<Point> newPoint) {
    LeftHandle = newPoint;
    EnforceMode(true);
    BezierNodeChanged.Invoke();
}

void BezierNode::SetRightHandle(const glm::vec3& newPos) {
    RightHandle->GetPosition() = newPos;
    EnforceMode(false);
    BezierNodeChanged.Invoke();
}
void BezierNode::SetRightHandle(const std::shared_ptr<Point> newPoint) {
    RightHandle = newPoint;
    EnforceMode(false);
    BezierNodeChanged.Invoke();
}

void BezierNode::SetMode(HandleMode newMode) {
    Mode = newMode;
    if (Mode != HandleMode::Free) {
        EnforceMode(true);
        BezierNodeChanged.Invoke();
    }
}



void BezierNode::EnforceMode(bool isLeftChanged) {
    if (Mode == HandleMode::Free) return;

    glm::vec3& movedHandle = isLeftChanged ? LeftHandle->GetPosition() : RightHandle->GetPosition();
    glm::vec3& oppositeHandle = isLeftChanged ? RightHandle->GetPosition() : LeftHandle->GetPosition();

    glm::vec3 direction = CenterHandle->GetPosition() - movedHandle;
    float distanceMoved = glm::length(direction);
    float distanceOpposite = glm::length(CenterHandle->GetPosition() - oppositeHandle);

    if (distanceMoved < 0.0001f) return;

    glm::vec3 normalizedDir = direction / distanceMoved;

    if (Mode == HandleMode::Aligned) {
        oppositeHandle = CenterHandle->GetPosition() + normalizedDir * distanceOpposite;
    } 
    else if (Mode == HandleMode::Symmetric) {
        oppositeHandle = CenterHandle->GetPosition() + normalizedDir * distanceMoved;
    }
}