#include "model/bezier_curve.h"

#include <algorithm>
#include <vector>

int BezierCurve::GetSegmentCount() const {
    if (Nodes.size() < 2) return 0;
    return static_cast<int>(Nodes.size()) - 1;
}

void BezierCurve::AddNode(const BezierNode& node) {
    Nodes.push_back(node);
}

void BezierCurve::AddNode(const glm::vec3& position) {
    Nodes.emplace_back(position);
}

void BezierCurve::RemoveNode(const BezierNode& node) {
    //TODO Ez így nem feltétlen a legjobb
    int idx = -1;
    for (int i = 0; i < Nodes.size(); i++) {
        if (&Nodes[i] == &node) {
            idx = i;
            break;
        }
    }
    if (idx >= 0) {
        Nodes.erase(Nodes.begin() + idx);
    }
}

glm::vec3 BezierCurve::EvaluateSegment(int segmentIndex, float t) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return glm::vec3(0.0f);
    }

    const BezierNode& nodeA = Nodes[segmentIndex];
    const BezierNode& nodeB = Nodes[segmentIndex + 1];

    const glm::vec3& p0 = nodeA.GetPosition();
    const glm::vec3& p1 = nodeA.GetRightHandle();
    const glm::vec3& p2 = nodeB.GetLeftHandle();
    const glm::vec3& p3 = nodeB.GetPosition();

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    glm::vec3 point = uuu * p0;
    point += 3.0f * uu * t * p1;
    point += 3.0f * u * tt * p2;
    point += ttt * p3;

    return point;
}

glm::vec3 BezierCurve::EvaluateCurve(float t) const {
    int segmentIdx = t >= 1.0f ? GetSegmentCount() - 1 : (int)(GetSegmentCount() * t);
    float localT = t * GetSegmentCount() - segmentIdx;

    return EvaluateSegment(segmentIdx, localT);
}

std::vector<glm::vec3> BezierCurve::GenerateRenderPoints(int resolution) const {
    std::vector<glm::vec3> renderPoints;
    
    int segments = GetSegmentCount();
    if (segments == 0 && !Nodes.empty()) {
        renderPoints.push_back(Nodes[0].GetPosition());
        return renderPoints;
    }

    renderPoints.reserve(segments * resolution);

    for (int i = 0; i < segments; ++i) {
        int steps = (i == segments - 1) ? resolution : resolution - 1;

        for (int step = 0; step <= steps; ++step) {
            float t = static_cast<float>(step) / static_cast<float>(resolution);
            renderPoints.push_back(EvaluateSegment(i, t));
        }
    }

    return renderPoints;
}