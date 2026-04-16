#include "curve_mesh.h"
#include "model/element/i_Curve.hpp"

void CurveMesh::AddEdge(std::shared_ptr<BezierCurve> edge) {
    edges.push_back(edge);
    edge->CurveChanged += [&](){ CurveMeshChanged.Invoke(); };
    CurveMeshChanged.Invoke();
}

void CurveMesh::AddSurface(std::shared_ptr<CoonsSurface> surface) {
    surfaces.push_back(surface);
    surface->CoonsSurfaceChanged += [&](){ CurveMeshChanged.Invoke(); };
    CurveMeshChanged.Invoke();
}
