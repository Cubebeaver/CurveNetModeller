#include "coons_surface.h"

CoonsSurface::CoonsSurface(
    std::shared_ptr<HalfEdge> bottom, std::shared_ptr<HalfEdge> top,
    std::shared_ptr<HalfEdge> left, std::shared_ptr<HalfEdge> right
) : bottom(bottom), top(top), left(left), right(right) {
    this->bottom->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->top->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->left->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->right->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
}

glm::vec3 CoonsSurface::Evaluate(float u, float v) const {
    const auto& S_u0 = bottom->EvaluatePosition(u);
    const auto& S_u1 = top->EvaluatePosition(u);
    const auto& S_0v = left->EvaluatePosition(v);
    const auto& S_1v = right->EvaluatePosition(v);

    const auto& S1_uv = (1 - v) * S_u0 + v * S_u1;
    const auto& S2_uv = (1 - u) * S_0v + u * S_1v;

    const auto& S_00 = bottom->EvaluatePosition(0);
    const auto& S_10 = bottom->EvaluatePosition(1);
    const auto& S_01 = top->EvaluatePosition(0);
    const auto& S_11 = top->EvaluatePosition(1);

    const auto& S12_uv = (1 - v) * ((1 - u) * S_00 + u * S_10)
                             + v * ((1 - u) * S_01 + u * S_11);

    return S1_uv + S2_uv - S12_uv;
}

CoonsSurface::~CoonsSurface() {
    bottom->HalfEdgeChanged.RemoveListener(this, &CoonsSurface::OnHalfEdgeChanged);
    top->HalfEdgeChanged.RemoveListener(this, &CoonsSurface::OnHalfEdgeChanged);
    left->HalfEdgeChanged.RemoveListener(this, &CoonsSurface::OnHalfEdgeChanged);
    right->HalfEdgeChanged.RemoveListener(this, &CoonsSurface::OnHalfEdgeChanged);
}

void CoonsSurface::InitializeAfterLoad() {
    this->bottom->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->top->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->left->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
    this->right->HalfEdgeChanged.AddListener(this, &CoonsSurface::OnHalfEdgeChanged);
}

void CoonsSurface::OnHalfEdgeChanged() {
    CoonsSurfaceChanged.Invoke();
}
