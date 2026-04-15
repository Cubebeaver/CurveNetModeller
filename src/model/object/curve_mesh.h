#pragma once

#include <memory>
#include <vector>

#include "object.h"
#include "model/element/i_Curve.hpp"
#include "model/element/i_Surface.hpp"
#include "util/event.hpp"


class CurveMesh : public Object {
private:
    std::vector<std::shared_ptr<ICurve>> edges;
    std::vector<std::shared_ptr<ISurface>> surfaces;

public:
    Event<> CurveMeshChanged;

    const std::vector<std::shared_ptr<ICurve>>& GetEdges() const { return edges; }
    const std::vector<std::shared_ptr<ISurface>>& GetSurfaces() const { return surfaces; }

    void AddEdge(std::shared_ptr<ICurve> edge);
    void AddSurface(std::shared_ptr<ISurface> surface);

    void AddNewCoonsSurface();

    virtual ~CurveMesh() override = default;
};
