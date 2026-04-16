#pragma once

#include <memory>
#include <vector>

#include "object.h"
#include "model/element/bezier_curve.h"
#include "model/element/coons_surface.h"
#include "model/element/i_Curve.hpp"
#include "model/element/i_Surface.hpp"
#include "util/event.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>


class CurveMesh : public Object {
private:
    std::vector<std::shared_ptr<BezierCurve>> edges;
    std::vector<std::shared_ptr<CoonsSurface>> surfaces;

public:
    Event<> CurveMeshChanged;

    const std::vector<std::shared_ptr<BezierCurve>>& GetEdges() const { return edges; }
    const std::vector<std::shared_ptr<CoonsSurface>>& GetSurfaces() const { return surfaces; }

    void AddEdge(std::shared_ptr<BezierCurve> edge);
    void AddSurface(std::shared_ptr<CoonsSurface> surface);

    void AddNewCoonsSurface();

    virtual ~CurveMesh() override = default;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(edges), CEREAL_NVP(surfaces));
    }
};
