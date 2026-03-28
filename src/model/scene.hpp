#pragma once

#include "coons_surface.h"

class Scene {
public:
    std::vector<std::shared_ptr<CoonsSurface>> surfaces;
    std::vector<std::shared_ptr<BezierCurve>> curves;
    std::vector<std::shared_ptr<BezierNode>> nodes;

    Scene() { }

    void Add(const std::shared_ptr<CoonsSurface>& obj) {
        surfaces.push_back(obj);
    }
    void Add(const std::shared_ptr<BezierCurve>& obj) {
        curves.push_back(obj);
    }
    void Add(const std::shared_ptr<BezierNode>& obj) {
        nodes.push_back(obj);
    }
};
