//
// Created by Cubebeaver on 2026. 04. 03..
//

#include "CurveMesh.h"
void CurveMesh::AddEdge(std::shared_ptr<ISpline> edge) {
    edges.push_back(edge);
}

void CurveMesh::AddSurface(std::shared_ptr<ISurface> surface) {
    surfaces.push_back(surface);
}
