#include "bicubic_coons_surface.h"

BicubicCoonsSurface::BicubicCoonsSurface(std::shared_ptr<HalfEdge> bottom, std::shared_ptr<HalfEdge> top,
                                                std::shared_ptr<HalfEdge> left, std::shared_ptr<HalfEdge> right, int rmfResolution)
                                              : bottom(bottom), top(top), left(left), right(right), rmfResolution(rmfResolution) {
    this->bottom->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnBottomHalfEdgeChanged);
    this->top->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnTopHalfEdgeChanged);
    this->left->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnLeftHalfEdgeChanged);
    this->right->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnRightHalfEdgeChanged);

    UpdateAllRMF();
}

BicubicCoonsSurface::~BicubicCoonsSurface() {
    this->bottom->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnBottomHalfEdgeChanged);
    this->top->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnTopHalfEdgeChanged);
    this->left->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnLeftHalfEdgeChanged);
    this->right->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnRightHalfEdgeChanged);

    UpdateAllRMF();
}

void BicubicCoonsSurface::InitializeAfterLoad() {
    this->bottom->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnBottomHalfEdgeChanged);
    this->top->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnTopHalfEdgeChanged);
    this->left->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnLeftHalfEdgeChanged);
    this->right->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnRightHalfEdgeChanged);

    UpdateAllRMF();
}

glm::vec3 BicubicCoonsSurface::S_u0(float u) const {
    return bottom->EvaluatePosition(u);
}
glm::vec3 BicubicCoonsSurface::S_u1(float u) const {
    return top->EvaluatePosition(1 - u);
}
glm::vec3 BicubicCoonsSurface::S_0v(float v) const {
    return left->EvaluatePosition(1 - v);
}
glm::vec3 BicubicCoonsSurface::S_1v(float v) const {
    return right->EvaluatePosition(v);
}

glm::vec3 BicubicCoonsSurface::Sv_u0(float u) const { // Bottom
    RMFFrame f0 = RMF::Sample(cachedRmfBottom, 0.0f);
    RMFFrame f1 = RMF::Sample(cachedRmfBottom, 1.0f);
    glm::vec3 nbourVel0 = -left->EvaluateVelocity(1.0f);
    glm::vec3 nbourVel1 =  right->EvaluateVelocity(0.0f);

    float alphaB0 = glm::dot(nbourVel0, f0.t);
    float betaB0  = glm::dot(nbourVel0, f0.s);
    float alphaB1 = glm::dot(nbourVel1, f1.t);
    float betaB1  = glm::dot(nbourVel1, f1.s);

    float alphaB = (1.0f - u) * alphaB0 + u * alphaB1;
    float betaB  = (1.0f - u) * betaB0  + u * betaB1;
    RMFFrame fB  = RMF::Sample(cachedRmfBottom, u);
    glm::vec3 Sv_u0 = alphaB * fB.t + betaB * fB.s;
    return Sv_u0;
}
glm::vec3 BicubicCoonsSurface::Sv_u1(float u) const { // Top
    RMFFrame f0 = RMF::Sample(cachedRmfTop, 1.0f);
    RMFFrame f1 = RMF::Sample(cachedRmfTop, 0.0f);
    glm::vec3 nbourVel0  = left->EvaluateVelocity(0.0f);
    glm::vec3 nbourVel1 = -right->EvaluateVelocity(1.0f);

    float alphaB0 = glm::dot(nbourVel0, f0.t);
    float betaB0  = glm::dot(nbourVel0, f0.s);
    //float gammaB0 = glm::dot(nbourVel0, f0.r);

    float alphaB1 = glm::dot(nbourVel1, f1.t);
    float betaB1  = glm::dot(nbourVel1, f1.s);
    //float gammaB1 = glm::dot(nbourVel1, f1.r);

    float alphaB = (1.0f - u) * alphaB0 + u * alphaB1;
    float betaB  = (1.0f - u) * betaB0  + u * betaB1;
    //float gammaB = (1.0f - s) * gammaB0 + s * gammaB1

    RMFFrame fB  = RMF::Sample(cachedRmfTop, 1 - u);
    glm::vec3 Sv_u1 = alphaB * fB.t + betaB * fB.s /* + gammaB * f.r*/;
    return Sv_u1;
}
glm::vec3 BicubicCoonsSurface::Su_0v(float v) const { // Left
    RMFFrame f0 = RMF::Sample(cachedRmfLeft, 1.0f);
    RMFFrame f1 = RMF::Sample(cachedRmfLeft, 0.0f);
    glm::vec3 nbourVel0 =  bottom->EvaluateVelocity(0.0f);
    glm::vec3 nbourVel1 = -top->EvaluateVelocity(1.0f);

    float alphaB0 = glm::dot(nbourVel0, f0.t);
    float betaB0  = glm::dot(nbourVel0, f0.s);
    float alphaB1 = glm::dot(nbourVel1, f1.t);
    float betaB1  = glm::dot(nbourVel1, f1.s);

    float alphaB = (1.0f - v) * alphaB0 + v * alphaB1;
    float betaB  = (1.0f - v) * betaB0  + v * betaB1;
    RMFFrame fB  = RMF::Sample(cachedRmfLeft, 1 - v);
    glm::vec3 Sv_0v = alphaB * fB.t + betaB * fB.s;
    return Sv_0v;
}
glm::vec3 BicubicCoonsSurface::Su_1v(float v) const { // Right
    RMFFrame f0 = RMF::Sample(cachedRmfRight, 0.0f);
    RMFFrame f1 = RMF::Sample(cachedRmfRight, 1.0f);
    glm::vec3 nbourVel0 = -bottom->EvaluateVelocity(1.0f);
    glm::vec3 nbourVel1 =  top->EvaluateVelocity(0.0f);

    float alphaB0 = glm::dot(nbourVel0, f0.t);
    float betaB0  = glm::dot(nbourVel0, f0.s);
    float alphaB1 = glm::dot(nbourVel1, f1.t);
    float betaB1  = glm::dot(nbourVel1, f1.s);

    float alphaB = (1.0f - v) * alphaB0 + v * alphaB1;
    float betaB  = (1.0f - v) * betaB0  + v * betaB1;
    RMFFrame fB  = RMF::Sample(cachedRmfRight, v);
    glm::vec3 Sv_1v = alphaB * fB.t + betaB * fB.s;
    return Sv_1v;
}
glm::vec3 BicubicCoonsSurface::S1(float u, float v) const {
    return Hermite::F0(v) * S_u0(u) + Hermite::G0(v) * Sv_u0(u) + Hermite::F1(v) * S_u1(u) + Hermite::G1(v) * Sv_u1(u);
}
glm::vec3 BicubicCoonsSurface::S2(float u, float v) const {
    return Hermite::F0(u) * S_0v(v) + Hermite::G0(u) * Su_0v(v) + Hermite::F1(u) * S_1v(v) + Hermite::G1(u) * Su_1v(v);
}
glm::vec3 BicubicCoonsSurface::S12(float u, float v) const {
    return Hermite::F0(u) * Hermite::F0(v) * S_u0(0)  + Hermite::G0(u) * Hermite::F0(v) * Su_0v(0) + Hermite::F1(u) * Hermite::F0(v) * S_u0(1)  + Hermite::G1(u) * Hermite::F0(v) * Su_1v(0)
        + Hermite::F0(u) * Hermite::G0(v) * Sv_u0(0)    /*              Zero twist              */ + Hermite::F1(u) * Hermite::G0(v) * Sv_u0(1)   /*              Zero twist              */
        + Hermite::F0(u) * Hermite::F1(v) * S_u1(0)   + Hermite::G0(u) * Hermite::F1(v) * Su_0v(1) + Hermite::F1(u) * Hermite::F1(v) * S_u1(1)  + Hermite::G1(u) * Hermite::F1(v) * Su_1v(1)
        + Hermite::F0(u) * Hermite::G1(v) * Sv_u1(0)    /*              Zero twist              */ + Hermite::F1(u) * Hermite::G1(v) * Sv_u1(1)   /*              Zero twist              */;
}

glm::vec3 BicubicCoonsSurface::Evaluate(float u, float v) const {
    if (!bottom || !top || !left || !right) return glm::vec3(0.0f);

    // Lazy-loading az RMF keretekhez, ha még nincsenek legenerálva
    if (cachedRmfBottom.empty() || cachedRmfTop.empty() ||
        cachedRmfLeft.empty() || cachedRmfRight.empty()) {
        UpdateAllRMF();
    }

    return S1(u, v) + S2(u, v) - S12(u, v);
}

void BicubicCoonsSurface::UpdateAllRMF() const {
    UpdateBottomRMF();
    UpdateTopRMF();
    UpdateLeftRMF();
    UpdateRightRMF();
}

void BicubicCoonsSurface::UpdateBottomRMF() const {
    if (bottom && bottom->curve.lock())
        cachedRmfBottom = RMF::GenerateFrames(bottom, defaultStartNormal, defaultEndNormal, rmfResolution);
}
void BicubicCoonsSurface::UpdateTopRMF() const {
    if (top && top->curve.lock())
        cachedRmfTop = RMF::GenerateFrames(top, defaultStartNormal, defaultEndNormal, rmfResolution);
}
void BicubicCoonsSurface::UpdateLeftRMF() const {
    if (left && left->curve.lock())
        cachedRmfLeft = RMF::GenerateFrames(left, defaultStartNormal, defaultEndNormal, rmfResolution);
}
void BicubicCoonsSurface::UpdateRightRMF() const {
    if (right && right->curve.lock())
        cachedRmfRight = RMF::GenerateFrames(right, defaultStartNormal, defaultEndNormal, rmfResolution);
}

void BicubicCoonsSurface::OnBottomHalfEdgeChanged() {
    UpdateBottomRMF();
    SurfaceChanged.Invoke();
}
void BicubicCoonsSurface::OnTopHalfEdgeChanged() {
    UpdateTopRMF();
    SurfaceChanged.Invoke();
}
void BicubicCoonsSurface::OnLeftHalfEdgeChanged() {
    UpdateLeftRMF();
    SurfaceChanged.Invoke();
}
void BicubicCoonsSurface::OnRightHalfEdgeChanged() {
    UpdateRightRMF();
    SurfaceChanged.Invoke();
}