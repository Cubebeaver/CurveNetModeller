#include "curve_mesh.h"
#include "model/element/i_Curve.hpp"

CurveMesh::CurveMesh() : Object() {

}

void CurveMesh::AddPoint(std::shared_ptr<Point> point) {
    points.push_back(point);
    point->PointChanged.AddListener(this, &CurveMesh::OnPointChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemovePoint(std::shared_ptr<Point> point) {
    points.erase(std::remove(points.begin(), points.end(), point), points.end());
    point->PointChanged.RemoveListener(this, &CurveMesh::OnPointChanged);
    CurveMeshChanged.Invoke();
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

void CurveMesh::AddSurface(std::shared_ptr<ISurface> surface) {
    surfaces.push_back(surface);
    surface->CoonsSurfaceChanged.AddListener(this, &CurveMesh::OnSurfaceChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::RemoveSurface(std::shared_ptr<ISurface> surface) {
    surfaces.erase(std::remove(surfaces.begin(), surfaces.end(), surface), surfaces.end());
    surface->CoonsSurfaceChanged.RemoveListener(this, &CurveMesh::OnSurfaceChanged);
    CurveMeshChanged.Invoke();
}

void CurveMesh::InitializeAfterLoad() {
    for (auto& point : points) {
        point->PointChanged.AddListener(this, &CurveMesh::OnPointChanged);
        point->InitializeAfterLoad();
    }

    for (auto& edge : edges) {
        edge->CurveChanged.AddListener(this, &CurveMesh::OnEdgeChanged);
        edge->InitializeAfterLoad();
    }

    for (auto& face : surfaces) {
        face->CoonsSurfaceChanged.AddListener(this, &CurveMesh::OnSurfaceChanged);
        face->InitializeAfterLoad();
    }
}

void CurveMesh::OnPointChanged(glm::vec3 offset) {
    CurveMeshChanged.Invoke();
}
void CurveMesh::OnEdgeChanged() {
    CurveMeshChanged.Invoke();
}
void CurveMesh::OnSurfaceChanged() {
    CurveMeshChanged.Invoke();
}
