#include "model/bezier_node.h"



BezierNode::BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode) 
    : Position(position), LeftHandle(leftHandle), RightHandle(rightHandle), Mode(mode) { }

BezierNode::BezierNode(HandleMode mode) 
    : BezierNode(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(1, 0, 0), mode) { }

BezierNode::BezierNode(glm::vec3 position, HandleMode mode)
    : BezierNode(position, position + glm::vec3(-1, 0, 0), position + glm::vec3(1, 0, 0), mode) { }

void BezierNode::SetPosition(const glm::vec3& newPos) {
    glm::vec3 delta = newPos - Position;
    Position = newPos;
    LeftHandle += delta;
    RightHandle += delta;
}

void BezierNode::SetLeftHandle(const glm::vec3& newPos) {
    LeftHandle = newPos;
    EnforceMode(true);
}

void BezierNode::SetRightHandle(const glm::vec3& newPos) {
    RightHandle = newPos;
    EnforceMode(false);
}

void BezierNode::SetMode(HandleMode newMode) {
    Mode = newMode;
    if (Mode != HandleMode::Free) {
        EnforceMode(true);
    }
}



void BezierNode::EnforceMode(bool isLeftChanged) {
    if (Mode == HandleMode::Free) return;

    glm::vec3& movedHandle = isLeftChanged ? LeftHandle : RightHandle;
    glm::vec3& oppositeHandle = isLeftChanged ? RightHandle : LeftHandle;

    glm::vec3 direction = Position - movedHandle;
    float distanceMoved = glm::length(direction);
    float distanceOpposite = glm::length(Position - oppositeHandle);

    if (distanceMoved < 0.0001f) return;

    glm::vec3 normalizedDir = direction / distanceMoved;

    if (Mode == HandleMode::Aligned) {
        oppositeHandle = Position + normalizedDir * distanceOpposite;
    } 
    else if (Mode == HandleMode::Symmetric) {
        oppositeHandle = Position + normalizedDir * distanceMoved;
    }
}