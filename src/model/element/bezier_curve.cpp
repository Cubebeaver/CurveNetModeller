#include "bezier_curve.h"

#include <algorithm>
#include <vector>

int BezierCurve::GetSegmentCount() const {
    if (Nodes.size() < 2) return 0;
    return static_cast<int>(Nodes.size()) - 1;
}

void BezierCurve::AddNode(std::shared_ptr<BezierNode> node) {
    Nodes.push_back(node);
    node->BezierNodeChanged.AddListener(this, &BezierCurve::OnChange);
    CurveChanged.Invoke();
}

void BezierCurve::AddNodeAt(std::shared_ptr<BezierNode> node, int index) {
    if (index < 0 || index > static_cast<int>(Nodes.size())) return;

    Nodes.insert(Nodes.begin() + index, node);
    node->BezierNodeChanged.AddListener(this, &BezierCurve::OnChange);
    CurveChanged.Invoke();
}

void BezierCurve::RemoveNodeAt(int idx) {
    if (idx < 0 || Nodes.size() <= idx) return;

    Nodes[idx]->BezierNodeChanged.RemoveListener(this, &BezierCurve::OnChange);
    Nodes.erase(Nodes.begin() + idx);
    CurveChanged.Invoke();
}

void BezierCurve::RemoveNode(std::weak_ptr<BezierNode> node) {
    auto n = node.lock();
    if (!n) return;

    n->BezierNodeChanged.RemoveListener(this, &BezierCurve::OnChange);
    Nodes.erase(std::remove(Nodes.begin(), Nodes.end(), n), Nodes.end());
    CurveChanged.Invoke();
}

int BezierCurve::IndexOf(std::weak_ptr<BezierNode> node) const {
    auto n = node.lock();
    if (!n) return -1;

    auto it = std::find(Nodes.begin(), Nodes.end(), n);
    if (it != Nodes.end()) {
        return static_cast<int>(std::distance(Nodes.begin(), it));
    }
    return -1;
}

glm::vec3 BezierCurve::EvaluateSegment(int segmentIndex, float t) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return glm::vec3(0.0f);
    }

    const BezierNode& nodeA = *Nodes[segmentIndex];
    const BezierNode& nodeB = *Nodes[segmentIndex + 1];

    const glm::vec3& p0 = nodeA.GetCenterHandle()->GetPosition();
    const glm::vec3& p1 = nodeA.GetRightHandle()->GetPosition();
    const glm::vec3& p2 = nodeB.GetLeftHandle()->GetPosition();
    const glm::vec3& p3 = nodeB.GetCenterHandle()->GetPosition();

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
float BezierCurve::EvaluateSegmentCurvature(int segmentIndex, float t) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return 0;
    }

    const BezierNode& nodeA = *Nodes[segmentIndex];
    const BezierNode& nodeB = *Nodes[segmentIndex + 1];

    const glm::vec3& p0 = nodeA.GetCenterHandle()->GetPosition();
    const glm::vec3& p1 = nodeA.GetRightHandle()->GetPosition();
    const glm::vec3& p2 = nodeB.GetLeftHandle()->GetPosition();
    const glm::vec3& p3 = nodeB.GetCenterHandle()->GetPosition();

    float u = 1.0f - t;

    glm::vec3 d1 = 3.0f * u * u * (p1 - p0) +
                   6.0f * u * t * (p2 - p1) +
                   3.0f * t * t * (p3 - p2);

    glm::vec3 d2 = 6.0f * u * (p2 - 2.0f * p1 + p0) +
                   6.0f * t * (p3 - 2.0f * p2 + p1);

    float crossLength = glm::length(glm::cross(d1, d2));

    float velocityLength = glm::length(d1);

    if (velocityLength < 0.00001f) {
        return 0.0f;
    }

    float velocityCubed = velocityLength * velocityLength * velocityLength;
    return crossLength / velocityCubed;
}
glm::vec3 BezierCurve::EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return glm::vec3(0, 0, 0);
    }

    const BezierNode& nodeA = *Nodes[segmentIndex];
    const BezierNode& nodeB = *Nodes[segmentIndex + 1];

    const glm::vec3& p0 = nodeA.GetCenterHandle()->GetPosition();
    const glm::vec3& p1 = nodeA.GetRightHandle()->GetPosition();
    const glm::vec3& p2 = nodeB.GetLeftHandle()->GetPosition();
    const glm::vec3& p3 = nodeB.GetCenterHandle()->GetPosition();

    float u = 1.0f - t;

    glm::vec3 d1 = 3.0f * u * u * (p1 - p0) +
                   6.0f * u * t * (p2 - p1) +
                   3.0f * t * t * (p3 - p2);

    glm::vec3 d2 = 6.0f * u * (p2 - 2.0f * p1 + p0) +
                   6.0f * t * (p3 - 2.0f * p2 + p1);

    if (glm::length(d1) < 0.00001f) {
        return glm::vec3(0.0f, 1.0f, 0.0f); // Fallback felfelé
    }

    glm::vec3 binormal = glm::cross(d1, d2);

    if (glm::length(binormal) < 0.00001f) {
        glm::vec3 up(0.0f, 1.0f, 0.0f);
        if (std::abs(glm::dot(glm::normalize(d1), up)) > 0.99f) {
            up = glm::vec3(1.0f, 0.0f, 0.0f);
        }
        return glm::normalize(glm::cross(d1, up));
    }

    glm::vec3 normal = glm::cross(binormal, d1);

    return glm::normalize(normal);
}
glm::vec3 BezierCurve::EvaluateSegmentCameraNormal(int segmentIndex, float t, glm::vec3 cam) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return glm::vec3(0, 0, 0);
    }

    const BezierNode& nodeA = *Nodes[segmentIndex];
    const BezierNode& nodeB = *Nodes[segmentIndex + 1];

    const glm::vec3& p0 = nodeA.GetCenterHandle()->GetPosition();
    const glm::vec3& p1 = nodeA.GetRightHandle()->GetPosition();
    const glm::vec3& p2 = nodeB.GetLeftHandle()->GetPosition();
    const glm::vec3& p3 = nodeB.GetCenterHandle()->GetPosition();

    float u = 1.0f - t;

    glm::vec3 d1 = 3.0f * u * u * (p1 - p0) +
                   6.0f * u * t * (p2 - p1) +
                   3.0f * t * t * (p3 - p2);

    glm::vec3 d2 = cam;

    glm::vec3 normal = glm::cross(d1, d2);

    return glm::normalize(normal);
}

glm::vec3 BezierCurve::EvaluatePosition(float t) const {
    int segmentIdx = t >= 1.0f ? GetSegmentCount() - 1 : static_cast<int>(GetSegmentCount() * t);
    float localT = t * GetSegmentCount() - segmentIdx;

    return EvaluateSegment(segmentIdx, localT);
}

float BezierCurve::EvaluateCurveCurvature(float t) const {
    int segmentIdx = t >= 1.0f ? GetSegmentCount() - 1 : static_cast<int>(GetSegmentCount() * t);
    float localT = t * GetSegmentCount() - segmentIdx;

    return EvaluateSegmentCurvature(segmentIdx, localT);
}
glm::vec3 BezierCurve::EvaluateCurvePrincipalNormal(float t) const {
    int segmentIdx = t >= 1.0f ? GetSegmentCount() - 1 : static_cast<int>(GetSegmentCount() * t);
    float localT = t * GetSegmentCount() - segmentIdx;

    return EvaluateSegmentPrincipalNormal(segmentIdx, localT);
}

std::vector<glm::vec3> BezierCurve::GenerateRenderPoints(int resolution) const {
    std::vector<glm::vec3> renderPoints;

    int segments = GetSegmentCount();
    if (segments == 0 && !Nodes.empty()) {
        renderPoints.push_back(glm::vec3(0, 0, 0));
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

std::vector<glm::vec3> BezierCurve::GenerateRenderNormals(int resolution) const {
    std::vector<glm::vec3> renderPoints;

    int segments = GetSegmentCount();
    if (segments == 0 && !Nodes.empty()) {
        renderPoints.push_back(glm::vec3(0, 1, 0));
        return renderPoints;
    }

    renderPoints.reserve(segments * resolution);

    for (int i = 0; i < segments; ++i) {
        int steps = (i == segments - 1) ? resolution : resolution - 1;

        for (int step = 0; step <= steps; ++step) {
            float t = static_cast<float>(step) / static_cast<float>(resolution);
            renderPoints.push_back(EvaluateSegmentPrincipalNormal(i, t));
        }
    }

    return renderPoints;
}
std::vector<glm::vec3> BezierCurve::GenerateRenderCameraNormals(int resolution, glm::vec3 cam) const {
    std::vector<glm::vec3> renderPoints;

    int segments = GetSegmentCount();
    if (segments == 0 && !Nodes.empty()) {
        renderPoints.push_back(glm::vec3(0, 1, 0));
        return renderPoints;
    }

    renderPoints.reserve(segments * resolution);

    for (int i = 0; i < segments; ++i) {
        int steps = (i == segments - 1) ? resolution : resolution - 1;

        for (int step = 0; step <= steps; ++step) {
            float t = static_cast<float>(step) / static_cast<float>(resolution);
            renderPoints.push_back(EvaluateSegmentCameraNormal(i, t, cam));
        }
    }

    return renderPoints;
}
std::vector<float> BezierCurve::GenerateRenderCurvatures(int resolution) const {
    std::vector<float> renderPoints;

    int segments = GetSegmentCount();
    if (segments == 0 && !Nodes.empty()) {
        renderPoints.push_back(0);
        return renderPoints;
    }

    renderPoints.reserve(segments * resolution);

    for (int i = 0; i < segments; ++i) {
        int steps = (i == segments - 1) ? resolution : resolution - 1;

        for (int step = 0; step <= steps; ++step) {
            float t = static_cast<float>(step) / static_cast<float>(resolution);
            renderPoints.push_back(EvaluateSegmentCurvature(i, t));
        }
    }

    return renderPoints;
}

BezierCurve::~BezierCurve() {
    for (auto& node : Nodes) {
        node->BezierNodeChanged.RemoveListener(this, &BezierCurve::OnChange);
    }
}

void BezierCurve::OnChange() {
    CurveChanged.Invoke();
}
