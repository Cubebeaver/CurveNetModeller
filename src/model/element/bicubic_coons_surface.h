#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

#include "model/element/half_edge.h"
#include "model/element/i_Curve.hpp"
#include "model/element/i_Surface.hpp"
#include "model/element/rotation_minimizing_frame.hpp"

namespace Hermite {
    inline float F0(float t) { return 1.0f - 3.0f * t * t + 2.0f * t * t * t; }
    inline float F1(float t) { return 3.0f * t * t - 2.0f * t * t * t; }
    inline float G0(float t) { return t - 2.0f * t * t + t * t * t; }
    inline float G1(float t) { return -t * t + t * t * t; }
}



class BicubicCoonsSurface : public ISurface {
private:
    void OnBottomHalfEdgeChanged();

    void OnTopHalfEdgeChanged();

    void OnLeftHalfEdgeChanged();

    void OnRightHalfEdgeChanged();

public:
    std::shared_ptr<HalfEdge> bottom; // S(u,0)
    std::shared_ptr<HalfEdge> top;    // S(u,1)
    std::shared_ptr<HalfEdge> left;   // S(0,v)
    std::shared_ptr<HalfEdge> right;  // S(1,v)

    int rmfResolution = 50;
    glm::vec3 defaultStartNormal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 defaultEndNormal   = glm::vec3(0.0f, 1.0f, 0.0f);

    BicubicCoonsSurface() = default;
    BicubicCoonsSurface(std::shared_ptr<HalfEdge> bottom, std::shared_ptr<HalfEdge> top,
                        std::shared_ptr<HalfEdge> left, std::shared_ptr<HalfEdge> right,
                        int rmfResolution = 50);

    virtual ~BicubicCoonsSurface() override;;

    void InitializeAfterLoad() override;

    glm::vec3 S_u0(float u) const;
    glm::vec3 S_u1(float u) const;
    glm::vec3 S_0v(float v) const;
    glm::vec3 S_1v(float v) const;

    glm::vec3 Sv_u0(float u) const;
    glm::vec3 Sv_u1(float u) const;
    glm::vec3 Su_0v(float v) const;
    glm::vec3 Su_1v(float v) const;

    glm::vec3 S1(float u, float v) const;
    glm::vec3 S2(float u, float v) const;
    glm::vec3 S12(float u, float v) const;

    // Generálja az RMF kereteket a 4 határgörbére (érdemes meghívni ha a görbék megváltoznak)
    void UpdateAllRMF() const;
    void UpdateBottomRMF() const;
    void UpdateTopRMF() const;
    void UpdateLeftRMF() const;
    void UpdateRightRMF() const;

    glm::vec3 Evaluate(float u, float v) const override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(bottom), CEREAL_NVP(right), CEREAL_NVP(top), CEREAL_NVP(left), CEREAL_NVP(rmfResolution));
    }

private:
    mutable std::vector<RMFFrame> cachedRmfBottom;
    mutable std::vector<RMFFrame> cachedRmfTop;
    mutable std::vector<RMFFrame> cachedRmfLeft;
    mutable std::vector<RMFFrame> cachedRmfRight;
};
