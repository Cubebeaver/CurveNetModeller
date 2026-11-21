#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

#include "model/element/half_edge.hpp"
#include "model/element/i_Curve.hpp"
#include "model/element/i_Surface.hpp"
#include "model/element/rotation_minimizing_frame.hpp"

namespace Hermite {
    inline float F0(float t) { return 1.0f - 3.0f * t * t + 2.0f * t * t * t; }
    inline float F1(float t) { return 3.0f * t * t - 2.0f * t * t * t; }
    inline float G0(float t) { return t - 2.0f * t * t + t * t * t; }
    inline float G1(float t) { return -t * t + t * t * t; }
}

inline RMFFrame SampleRMF(const std::vector<RMFFrame>& frames, float curveT) {
    if (frames.empty()) return RMFFrame();
    if (frames.size() == 1) return frames[0];

    curveT = glm::clamp(curveT, 0.0f, 1.0f);
    float scaledT = curveT * static_cast<float>(frames.size() - 1);
    int idx = static_cast<int>(std::floor(scaledT));
    int nextIdx = std::min(idx + 1, static_cast<int>(frames.size()) - 1);
    float factor = scaledT - static_cast<float>(idx);

    RMFFrame result;
    result.position = glm::mix(frames[idx].position, frames[nextIdx].position, factor);
    result.r        = glm::normalize(glm::mix(frames[idx].r, frames[nextIdx].r, factor));
    result.s        = glm::normalize(glm::mix(frames[idx].s, frames[nextIdx].s, factor));
    result.t        = glm::normalize(glm::mix(frames[idx].t, frames[nextIdx].t, factor));
    return result;
}



class BicubicCoonsSurface : public ISurface {
private:
    void OnHalfEdgeChanged() {
        UpdateRMF();
        SurfaceChanged.Invoke();
    }

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
                        int rmfResolution = 50)
        : bottom(bottom), top(top), left(left), right(right), rmfResolution(rmfResolution) {}

    virtual ~BicubicCoonsSurface() override {
        this->bottom->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->top->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->left->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->right->HalfEdgeChanged.RemoveListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
    };

    void InitializeAfterLoad() override {
        this->bottom->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->top->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->left->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->right->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);

        UpdateRMF();
    }

    inline glm::vec3 S_u0(float u) const {
        return bottom->EvaluatePosition(u);
    }

    inline glm::vec3 S_u1(float u) const {
        return top->EvaluatePosition(1 - u);
    }

    inline glm::vec3 S_0v(float v) const {
        return left->EvaluatePosition(1 - v);
    }

    inline glm::vec3 S_1v(float v) const {
        return right->EvaluatePosition(v);
    }


    inline glm::vec3 Sv_u0(float u) const { // Bottom
        RMFFrame f0 = SampleRMF(cachedRmfBottom, 0.0f);
        RMFFrame f1 = SampleRMF(cachedRmfBottom, 1.0f);
        glm::vec3 nbourVel0 = -left->EvaluateVelocity(1.0f);
        glm::vec3 nbourVel1 =  right->EvaluateVelocity(0.0f);

        float alphaB0 = glm::dot(nbourVel0, f0.t);
        float betaB0  = glm::dot(nbourVel0, f0.s);
        float alphaB1 = glm::dot(nbourVel1, f1.t);
        float betaB1  = glm::dot(nbourVel1, f1.s);

        float alphaB = (1.0f - u) * alphaB0 + u * alphaB1;
        float betaB  = (1.0f - u) * betaB0  + u * betaB1;
        RMFFrame fB  = SampleRMF(cachedRmfBottom, u);
        glm::vec3 Sv_u0 = alphaB * fB.t + betaB * fB.s;
        return Sv_u0;
    }

    inline glm::vec3 Sv_u1(float u) const { // Top
        RMFFrame f0 = SampleRMF(cachedRmfTop, 1.0f);
        RMFFrame f1 = SampleRMF(cachedRmfTop, 0.0f);
        glm::vec3 nbourVel0  = left->EvaluateVelocity(0.0f);
        glm::vec3 nbourVel1 = -right->EvaluateVelocity(1.0f);

        float alphaB0 = glm::dot(nbourVel0, f0.t);
        float betaB0  = glm::dot(nbourVel0, f0.s);
        float alphaB1 = glm::dot(nbourVel1, f1.t);
        float betaB1  = glm::dot(nbourVel1, f1.s);

        float alphaB = (1.0f - u) * alphaB0 + u * alphaB1;
        float betaB  = (1.0f - u) * betaB0  + u * betaB1;
        RMFFrame fB  = SampleRMF(cachedRmfTop, 1 - u);
        glm::vec3 Sv_u1 = alphaB * fB.t + betaB * fB.s;
        return Sv_u1;
    }

    inline glm::vec3 Su_0v(float v) const { // Left
        RMFFrame f0 = SampleRMF(cachedRmfLeft, 1.0f);
        RMFFrame f1 = SampleRMF(cachedRmfLeft, 0.0f);
        glm::vec3 nbourVel0 =  bottom->EvaluateVelocity(0.0f);
        glm::vec3 nbourVel1 = -top->EvaluateVelocity(1.0f);

        float alphaB0 = glm::dot(nbourVel0, f0.t);
        float betaB0  = glm::dot(nbourVel0, f0.s);
        float alphaB1 = glm::dot(nbourVel1, f1.t);
        float betaB1  = glm::dot(nbourVel1, f1.s);

        float alphaB = (1.0f - v) * alphaB0 + v * alphaB1;
        float betaB  = (1.0f - v) * betaB0  + v * betaB1;
        RMFFrame fB  = SampleRMF(cachedRmfLeft, 1 - v);
        glm::vec3 Sv_0v = alphaB * fB.t + betaB * fB.s;
        return Sv_0v;
    }

    inline glm::vec3 Su_1v(float v) const { // Right
        RMFFrame f0 = SampleRMF(cachedRmfRight, 0.0f);
        RMFFrame f1 = SampleRMF(cachedRmfRight, 1.0f);
        glm::vec3 nbourVel0 = -bottom->EvaluateVelocity(1.0f);
        glm::vec3 nbourVel1 =  top->EvaluateVelocity(0.0f);

        float alphaB0 = glm::dot(nbourVel0, f0.t);
        float betaB0  = glm::dot(nbourVel0, f0.s);
        float alphaB1 = glm::dot(nbourVel1, f1.t);
        float betaB1  = glm::dot(nbourVel1, f1.s);

        float alphaB = (1.0f - v) * alphaB0 + v * alphaB1;
        float betaB  = (1.0f - v) * betaB0  + v * betaB1;
        RMFFrame fB  = SampleRMF(cachedRmfRight, v);
        glm::vec3 Sv_1v = alphaB * fB.t + betaB * fB.s;
        return Sv_1v;
    }



    inline glm::vec3 S1(float u, float v) const {
        return Hermite::F0(v) * S_u0(u) + Hermite::G0(v) * Sv_u0(u) + Hermite::F1(v) * S_u1(u) + Hermite::G1(v) * Sv_u1(u);
    }

    inline glm::vec3 S2(float u, float v) const {
        return Hermite::F0(u) * S_0v(v) + Hermite::G0(u) * Su_0v(v) + Hermite::F1(u) * S_1v(v) + Hermite::G1(u) * Su_1v(v);
    }

    inline glm::vec3 S12(float u, float v) const {
        return Hermite::F0(u) * Hermite::F0(v) * S_u0(0)  + Hermite::G0(u) * Hermite::F0(v) * Su_0v(0) + Hermite::F1(u) * Hermite::F0(v) * S_u0(1)  + Hermite::G1(u) * Hermite::F0(v) * Su_1v(0)
             + Hermite::F0(u) * Hermite::G0(v) * Sv_u0(0)   /*              Zero twist              */ + Hermite::F1(u) * Hermite::G0(v) * Sv_u0(1)   /*              Zero twist              */
             + Hermite::F0(u) * Hermite::F1(v) * S_u1(0)  + Hermite::G0(u) * Hermite::F1(v) * Su_0v(1) + Hermite::F1(u) * Hermite::F1(v) * S_u1(1)  + Hermite::G1(u) * Hermite::F1(v) * Su_1v(1)
             + Hermite::F0(u) * Hermite::G1(v) * Sv_u1(0)   /*              Zero twist              */ + Hermite::F1(u) * Hermite::G1(v) * Sv_u1(1)   /*              Zero twist              */;
    }

    // Generálja az RMF kereteket a 4 határgörbére (érdemes meghívni ha a görbék megváltoznak)
    void UpdateRMF() const {
        if (bottom && bottom->curve.lock())
            cachedRmfBottom = RotationMinimizingFrame::GenerateFrames(bottom, defaultStartNormal, defaultEndNormal, rmfResolution);
        if (top && top->curve.lock())
            cachedRmfTop = RotationMinimizingFrame::GenerateFrames(top, defaultStartNormal, defaultEndNormal, rmfResolution);
        if (left && left->curve.lock())
            cachedRmfLeft = RotationMinimizingFrame::GenerateFrames(left, defaultStartNormal, defaultEndNormal, rmfResolution);
        if (right && right->curve.lock())
            cachedRmfRight = RotationMinimizingFrame::GenerateFrames(right, defaultStartNormal, defaultEndNormal, rmfResolution);
    }

    glm::vec3 Evaluate(float u, float v) const override {
        if (!bottom || !top || !left || !right) return glm::vec3(0.0f);

        // Lazy-loading az RMF keretekhez, ha még nincsenek legenerálva
        if (cachedRmfBottom.empty() || cachedRmfTop.empty() ||
            cachedRmfLeft.empty() || cachedRmfRight.empty()) {
            UpdateRMF();
        }

        return S1(u, v) + S2(u, v) - S12(u, v);
    }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(bottom), CEREAL_NVP(right), CEREAL_NVP(top), CEREAL_NVP(left), CEREAL_NVP(rmfResolution));
    }

private:
    // Gyorsítótár (cache) az RMF keretekhez, hogy ne generálja újra 50 pontot minden Evaluate-nél
    mutable std::vector<RMFFrame> cachedRmfBottom;
    mutable std::vector<RMFFrame> cachedRmfTop;
    mutable std::vector<RMFFrame> cachedRmfLeft;
    mutable std::vector<RMFFrame> cachedRmfRight;
};