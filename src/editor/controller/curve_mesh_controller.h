#pragma once
#include <memory>

#include "editor/view/bezier_node_view.hpp"
#include "editor/view/curve_view.hpp"
#include "editor/view/coons_surface_view.hpp"
#include "../../model/object/curve_mesh.h"
#include "../../model/element/i_Node.hpp"
#include "../../model/element/point.h"

class CurveMeshController {
private:
    std::shared_ptr<CurveMesh> curveMesh;

    std::weak_ptr<ISurface> selectedSurface;
    std::weak_ptr<ICurve> selectedEdge;
    std::weak_ptr<INode> selectedNode;
    std::weak_ptr<Point> selectedPoint;

    std::vector<std::unique_ptr<BezierNodeView>> nodeViews;
    std::vector<std::unique_ptr<CurveView>> curveViews;
    std::vector<std::unique_ptr<CoonsSurfaceView>> surfaceViews;

public:
    CurveMeshController() {
        curveMesh = std::make_shared<CurveMesh>();

        curveMesh->CurveMeshChanged += [&]() {
            SyncViews();
        };
    }

private:
    void SyncViews() {
        auto& edges = curveMesh->GetEdges();
        auto& surfaces = curveMesh->GetSurfaces();

        for (int i = 0; i < edges.size(); i++) {
            curveViews[i]->Update(*edges[i], 50);
        }
    }
};
