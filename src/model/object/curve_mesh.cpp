#include "curve_mesh.h"
#include "model/element/i_Curve.hpp"

CurveMesh::CurveMesh() : Object() {

}

void CurveMesh::AddEdge(std::shared_ptr<BezierCurve> edge) {
    edges.push_back(edge);
    edge->CurveChanged.AddListener(this, &CurveMesh::OnEdgeChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemoveEdge(std::weak_ptr<BezierCurve> edge) {
    auto e = edge.lock();
    if (!e) return;

    edges.erase(std::remove(edges.begin(), edges.end(), e), edges.end());
    CurveMeshChanged.Invoke();
}

void CurveMesh::AddSurface(std::shared_ptr<CoonsSurface> surface) {
    surfaces.push_back(surface);
    surface->CoonsSurfaceChanged.AddListener(this, &CurveMesh::OnSurfaceChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemoveSurface(std::weak_ptr<CoonsSurface> surface) {
    auto s = surface.lock();
    if (!s) return;

    surfaces.erase(std::remove(surfaces.begin(), surfaces.end(), s), surfaces.end());
    CurveMeshChanged.Invoke();
}

void CurveMesh::OnEdgeChanged() {
    CurveMeshChanged.Invoke();
}
void CurveMesh::OnSurfaceChanged() {
    CurveMeshChanged.Invoke();
}
