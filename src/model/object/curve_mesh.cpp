//
// Created by Cubebeaver on 2026. 04. 03..
//

#include "curve_mesh.h"
#include "model/element/i_Curve.hpp"

void CurveMesh::AddEdge(std::shared_ptr<ICurve> edge) {
    edges.push_back(edge);
}

void CurveMesh::AddSurface(std::shared_ptr<ISurface> surface) {
    surfaces.push_back(surface);
}
