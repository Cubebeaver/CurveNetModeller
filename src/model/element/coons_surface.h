#pragma once

#include "bezier_curve.h"
#include <glm/glm.hpp>

#include "half_edge.hpp"
#include "i_Surface.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>


class CoonsSurface : public ISurface {
public:
    std::shared_ptr<HalfEdge> bottom; // old: c1
    std::shared_ptr<HalfEdge> right;  // old: d2
    std::shared_ptr<HalfEdge> top;    // old: c2
    std::shared_ptr<HalfEdge> left;   // old: d1

    CoonsSurface() { }
    CoonsSurface(std::shared_ptr<HalfEdge> bottom, std::shared_ptr<HalfEdge> top,
                 std::shared_ptr<HalfEdge> left, std::shared_ptr<HalfEdge> right);

    glm::vec3 Evaluate(float u, float v) const override;

    virtual ~CoonsSurface() override;

    void InitializeAfterLoad() override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(bottom), CEREAL_NVP(right), CEREAL_NVP(top), CEREAL_NVP(left));
    }

private:
    void OnHalfEdgeChanged();
};
