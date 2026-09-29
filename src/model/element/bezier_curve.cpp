#include "bezier_curve.h"

#include <algorithm>
#include <vector>

bool BezierCurve::GetSegmentControlPoints(int segmentIndex, glm::vec3& p0, glm::vec3& p1, glm::vec3& p2, glm::vec3& p3) const {
    if (segmentIndex < 0 || segmentIndex >= GetSegmentCount()) {
        return false;
    }
    const BezierNode& nodeA = *Nodes[segmentIndex];
    const BezierNode& nodeB = *Nodes[segmentIndex + 1];

    p0 = nodeA.GetCenterHandle()->GetPosition();
    p1 = nodeA.GetRightHandle()->GetPosition();
    p2 = nodeB.GetLeftHandle()->GetPosition();
    p3 = nodeB.GetCenterHandle()->GetPosition();
    return true;
}

void BezierCurve::GetLocalT(float t, int& segmentIdx, float& localT) const {
    int maxSegments = GetSegmentCount();
    if (maxSegments == 0) {
        segmentIdx = 0;
        localT = 0.0f;
        return;
    }
    segmentIdx = t >= 1.0f ? maxSegments - 1 : static_cast<int>(maxSegments * t);
    localT = t * maxSegments - segmentIdx;
}

// --- CSOMÓPONT KEZELÉS ---

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
    if (idx < 0 || Nodes.size() <= static_cast<size_t>(idx)) return;

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

// --- SZEGMENS ÉRTÉKELÉS (ISpline) ---

glm::vec3 BezierCurve::EvaluateSegment(int segmentIndex, float t) const {
    glm::vec3 p0, p1, p2, p3;
    if (!GetSegmentControlPoints(segmentIndex, p0, p1, p2, p3)) return glm::vec3(0.0f);

    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    return uuu * p0 + 3.0f * uu * t * p1 + 3.0f * u * tt * p2 + ttt * p3;
}

glm::vec3 BezierCurve::EvaluateSegmentVelocity(int segmentIndex, float t) const {
    glm::vec3 p0, p1, p2, p3;
    if (!GetSegmentControlPoints(segmentIndex, p0, p1, p2, p3)) return glm::vec3(0.0f);

    float u = 1.0f - t;

    return 3.0f * u * u * (p1 - p0) +
           6.0f * u * t * (p2 - p1) +
           3.0f * t * t * (p3 - p2);
}

glm::vec3 BezierCurve::EvaluateSegmentAcceleration(int segmentIndex, float t) const {
    glm::vec3 p0, p1, p2, p3;
    if (!GetSegmentControlPoints(segmentIndex, p0, p1, p2, p3)) return glm::vec3(0.0f);

    float u = 1.0f - t;

    return 6.0f * u * (p2 - 2.0f * p1 + p0) +
           6.0f * t * (p3 - 2.0f * p2 + p1);
}

float BezierCurve::EvaluateSegmentCurvature(int segmentIndex, float t) const {
    glm::vec3 d1 = EvaluateSegmentVelocity(segmentIndex, t);
    glm::vec3 d2 = EvaluateSegmentAcceleration(segmentIndex, t);

    float velocityLength = glm::length(d1);
    if (velocityLength < 0.00001f) {
        return 0.0f;
    }

    float crossLength = glm::length(glm::cross(d1, d2));
    float velocityCubed = velocityLength * velocityLength * velocityLength;

    return crossLength / velocityCubed;
}

glm::vec3 BezierCurve::EvaluateSegmentPrincipalNormal(int segmentIndex, float t) const {
    glm::vec3 d1 = EvaluateSegmentVelocity(segmentIndex, t);
    glm::vec3 d2 = EvaluateSegmentAcceleration(segmentIndex, t);

    if (glm::length(d1) < 0.00001f) {
        return glm::vec3(0.0f, 1.0f, 0.0f); // Fallback felfelé
    }

    glm::vec3 binormal = glm::cross(d1, d2);

    // Ha a binormális nulla (a görbe egyenes), keressünk egy tetszőleges merőlegest
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
    glm::vec3 d1 = EvaluateSegmentVelocity(segmentIndex, t);

    if (glm::length(d1) < 0.00001f) {
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }

    glm::vec3 normal = glm::cross(d1, cam);
    return glm::normalize(normal);
}

// --- GLOBÁLIS ÉRTÉKELÉS (ICurve) ---

glm::vec3 BezierCurve::EvaluatePosition(float t) const {
    int segmentIdx; float localT;
    GetLocalT(t, segmentIdx, localT);
    return EvaluateSegment(segmentIdx, localT);
}

glm::vec3 BezierCurve::EvaluateVelocity(float t) const {
    int segmentIdx; float localT;
    GetLocalT(t, segmentIdx, localT);
    return EvaluateSegmentVelocity(segmentIdx, localT);
}

glm::vec3 BezierCurve::EvaluateAcceleration(float t) const {
    int segmentIdx; float localT;
    GetLocalT(t, segmentIdx, localT);
    return EvaluateSegmentAcceleration(segmentIdx, localT);
}

float BezierCurve::EvaluateCurveCurvature(float t) const {
    int segmentIdx; float localT;
    GetLocalT(t, segmentIdx, localT);
    return EvaluateSegmentCurvature(segmentIdx, localT);
}

glm::vec3 BezierCurve::EvaluateCurvePrincipalNormal(float t) const {
    int segmentIdx; float localT;
    GetLocalT(t, segmentIdx, localT);
    return EvaluateSegmentPrincipalNormal(segmentIdx, localT);
}

// --- GENERÁTOROK ---

std::vector<glm::vec3> BezierCurve::GenerateRenderPoints(int resolution) const {
    return GenerateRenderData<glm::vec3>(resolution, glm::vec3(0.0f),
        [this](int i, float t) { return EvaluateSegment(i, t); });
}

std::vector<glm::vec3> BezierCurve::GenerateRenderNormals(int resolution) const {
    return GenerateRenderData<glm::vec3>(resolution, glm::vec3(0.0f, 1.0f, 0.0f),
        [this](int i, float t) { return EvaluateSegmentPrincipalNormal(i, t); });
}

std::vector<glm::vec3> BezierCurve::GenerateRenderCameraNormals(int resolution, glm::vec3 cam) const {
    return GenerateRenderData<glm::vec3>(resolution, glm::vec3(0.0f, 1.0f, 0.0f),
        [this, cam](int i, float t) { return EvaluateSegmentCameraNormal(i, t, cam); });
}

std::vector<float> BezierCurve::GenerateRenderCurvatures(int resolution) const {
    return GenerateRenderData<float>(resolution, 0.0f,
        [this](int i, float t) { return EvaluateSegmentCurvature(i, t); });
}

// --- ÉLETCIKLUS ÉS ESEMÉNYEK ---

void BezierCurve::InitializeAfterLoad() {
    for (auto& node : Nodes) {
        node->BezierNodeChanged.AddListener(this, &BezierCurve::OnChange);
        //TODO Ez így lehet nem a legjobb, de majd elv. úgyis ez csak egy contraint lesz???
        //TODO Ha mégse, akkor
        node->InitializeAfterLoad();
    }
}

BezierCurve::~BezierCurve() {
    for (auto& node : Nodes) {
        node->BezierNodeChanged.RemoveListener(this, &BezierCurve::OnChange);
    }
}

void BezierCurve::OnChange() {
    CurveChanged.Invoke();
}
