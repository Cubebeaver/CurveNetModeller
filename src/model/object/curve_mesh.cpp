#include "curve_mesh.h"
#include "model/element/i_Curve.hpp"

CurveMesh::CurveMesh() : Object() {

}

void CurveMesh::AddEdge(std::shared_ptr<BezierCurve> edge) {
    edges.push_back(edge);
    edge->CurveChanged.AddListener(this, &CurveMesh::OnEdgeChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemoveEdge(std::shared_ptr<BezierCurve> edge) {
    edges.erase(std::remove(edges.begin(), edges.end(), edge), edges.end());
    edge->CurveChanged.RemoveListener(this, &CurveMesh::OnEdgeChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::AddSurface(std::shared_ptr<CoonsSurface> surface) {
    surfaces.push_back(surface);
    surface->CoonsSurfaceChanged.AddListener(this, &CurveMesh::OnSurfaceChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemoveSurface(std::shared_ptr<CoonsSurface> surface) {
    surfaces.erase(std::remove(surfaces.begin(), surfaces.end(), surface), surfaces.end());
    surface->CoonsSurfaceChanged.RemoveListener(this, &CurveMesh::OnSurfaceChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::OnEdgeChanged() {
    CurveMeshChanged.Invoke();
}
void CurveMesh::OnSurfaceChanged() {
    CurveMeshChanged.Invoke();
}
