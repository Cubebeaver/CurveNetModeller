#include "model/element/bezier_node.h"


#define CenterHandle points[0]
#define LeftHandle points[1]
#define RightHandle points[2]

BezierNode::BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode) 
    : BezierNode(std::make_shared<Point>(position), std::make_shared<Point>(leftHandle), std::make_shared<Point>(rightHandle), mode) { }

BezierNode::BezierNode(std::shared_ptr<Point> position, std::shared_ptr<Point> leftHandle, std::shared_ptr<Point> rightHandle, HandleMode mode)
    : Mode(mode) {
    points.push_back(position);
    points.push_back(leftHandle);
    points.push_back(rightHandle);

    CenterHandle->PointChanged.AddListener(this, &BezierNode::OnCenterPointChanged);
    LeftHandle->PointChanged.AddListener(this, &BezierNode::OnLeftPointChanged);
    RightHandle->PointChanged.AddListener(this, &BezierNode::OnRightPointChanged);

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
//TODO Amikor kitörlöm a megosztott pont felül az egyik bezierNode-ot,
//     akkor annak az eventjei bent maradnak, és a törölt this már csak dangling pointer/ref -> crash
void BezierNode::SetPosition(const std::shared_ptr<Point> newPoint) {
    CenterHandle->PointChanged.RemoveListener(this, &BezierNode::OnCenterPointChanged);

    glm::vec3 delta = newPoint->GetPosition() - CenterHandle->GetPosition();
    CenterHandle = newPoint;
    LeftHandle->GetPosition() += delta;
    RightHandle->GetPosition() += delta;

    newPoint->PointChanged.AddListener(this, &BezierNode::OnCenterPointChanged);

    BezierNodeChanged.Invoke();
}


void BezierNode::SetLeftHandle(const glm::vec3& newPos) {
    LeftHandle->GetPosition() = newPos;
    EnforceMode(true);
    BezierNodeChanged.Invoke();
}
void BezierNode::SetLeftHandle(const std::shared_ptr<Point> newPoint) {
    LeftHandle->PointChanged.RemoveListener(this, &BezierNode::OnLeftPointChanged);

    LeftHandle = newPoint;
    EnforceMode(true);

    newPoint->PointChanged.AddListener(this, &BezierNode::OnLeftPointChanged);

    BezierNodeChanged.Invoke();
}

void BezierNode::SetRightHandle(const glm::vec3& newPos) {
    RightHandle->GetPosition() = newPos;
    EnforceMode(false);
    BezierNodeChanged.Invoke();
}
void BezierNode::SetRightHandle(const std::shared_ptr<Point> newPoint) {
    RightHandle->PointChanged.RemoveListener(this, &BezierNode::OnRightPointChanged);

    RightHandle = newPoint;
    EnforceMode(false);

    newPoint->PointChanged.AddListener(this, &BezierNode::OnRightPointChanged);

    BezierNodeChanged.Invoke();
}

void BezierNode::SetMode(HandleMode newMode) {
    if (Mode == newMode) return; //DEBUG??
    Mode = newMode;
    if (Mode != HandleMode::Free) {
        EnforceMode(true);
        BezierNodeChanged.Invoke();
    }
}

BezierNode::~BezierNode() {
    CenterHandle->PointChanged.RemoveListener(this, &BezierNode::OnCenterPointChanged);
    LeftHandle->PointChanged.RemoveListener(this, &BezierNode::OnLeftPointChanged);
    RightHandle->PointChanged.RemoveListener(this, &BezierNode::OnRightPointChanged);
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

void BezierNode::OnCenterPointChanged(const glm::vec3 offset) {
    LeftHandle->GetPosition() += offset;
    RightHandle->GetPosition() += offset;
    BezierNodeChanged.Invoke();
}

void BezierNode::OnLeftPointChanged(const glm::vec3 offset) {
    EnforceMode(true);
    BezierNodeChanged.Invoke();
}

void BezierNode::OnRightPointChanged(const glm::vec3 offset) {
    EnforceMode(false);
    BezierNodeChanged.Invoke();
}