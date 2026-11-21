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

// --- Hermite bázisfüggvények ---
namespace Hermite {
    inline float F0(float t) { return 1.0f - 3.0f * t * t + 2.0f * t * t * t; } // Pozíció t=0
    inline float F1(float t) { return 3.0f * t * t - 2.0f * t * t * t; }        // Pozíció t=1
    inline float G0(float t) { return t - 2.0f * t * t + t * t * t; }         // Derivált t=0
    inline float G1(float t) { return -t * t + t * t * t; }                 // Derivált t=1
}

// --- Segédfüggvények HalfEdge és RMF kezeléshez ---
inline float HalfEdgeLerp(float a, float b, float t) {
    return a + t * (b - a);
}

// Kiszámítja a HalfEdge deriváltját/sebességét (figyelembe véve a megfordítottságot)
inline glm::vec3 EvaluateHalfEdgeVelocity(const HalfEdge& edge, float t) {
    float newT = HalfEdgeLerp(edge.from, edge.to, t);
    if (auto s = edge.curve.lock()) {
        float scale = edge.to - edge.from;
        return scale * s->EvaluateVelocity(newT);
    }
    return glm::vec3(0.0f);
}

// RMF mintavételezése egy előre kiszámolt tömbből lineáris interpolációval
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

// Egy adott HalfEdge-hez tartozó RMF keret lekérése t paraméternél
inline RMFFrame GetFrameForHalfEdge(const HalfEdge& edge, const std::vector<RMFFrame>& curveFrames, float t) {
    float curveT = HalfEdgeLerp(edge.from, edge.to, t);
    RMFFrame frame = SampleRMF(curveFrames, curveT);

    // Ha a HalfEdge fordított irányú, az érintőt és a binormálist tükröznünk kell
    if (edge.from > edge.to) {
        frame.t = -frame.t;
        frame.s = -frame.s;
    }
    return frame;
}


// --- Bikubikus Coons Felület Osztály ---
class BicubicCoonsSurface : public ISurface {
private:
    void OnHalfEdgeChanged() {
        UpdateRMF();
        SurfaceChanged.Invoke();
    }

public:
    std::shared_ptr<HalfEdge> bottom; // C0(u), v = 0
    std::shared_ptr<HalfEdge> top;    // C1(u), v = 1
    std::shared_ptr<HalfEdge> left;   // D0(v), u = 0
    std::shared_ptr<HalfEdge> right;  // D1(v), u = 1

    int rmfResolution = 50;
    glm::vec3 defaultStartNormal = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 defaultEndNormal   = glm::vec3(0.0f, 1.0f, 0.0f);

    BicubicCoonsSurface() = default;
    BicubicCoonsSurface(std::shared_ptr<HalfEdge> bottom, std::shared_ptr<HalfEdge> top,
                        std::shared_ptr<HalfEdge> left, std::shared_ptr<HalfEdge> right,
                        int rmfResolution = 50)
        : bottom(bottom), top(top), left(left), right(right), rmfResolution(rmfResolution) {}

    virtual ~BicubicCoonsSurface() override = default;

    void InitializeAfterLoad() override {
        this->bottom->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->top->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->left->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);
        this->right->HalfEdgeChanged.AddListener(this, &BicubicCoonsSurface::OnHalfEdgeChanged);

        UpdateRMF();
    }

    // Generálja az RMF kereteket a 4 határgörbére (érdemes meghívni ha a görbék megváltoznak)
    void UpdateRMF() const {
        if (bottom && bottom->curve.lock())
            cachedRmfBottom = RotationMinimizingFrame::GenerateFrames(bottom->curve.lock(), defaultStartNormal, defaultEndNormal, rmfResolution);
        if (top && top->curve.lock())
            cachedRmfTop = RotationMinimizingFrame::GenerateFrames(top->curve.lock(), defaultStartNormal, defaultEndNormal, rmfResolution);
        if (left && left->curve.lock())
            cachedRmfLeft = RotationMinimizingFrame::GenerateFrames(left->curve.lock(), defaultStartNormal, defaultEndNormal, rmfResolution);
        if (right && right->curve.lock())
            cachedRmfRight = RotationMinimizingFrame::GenerateFrames(right->curve.lock(), defaultStartNormal, defaultEndNormal, rmfResolution);
    }

    glm::vec3 Evaluate(float u, float v) const override {
        if (!bottom || !top || !left || !right) return glm::vec3(0.0f);

        // Lazy-loading az RMF keretekhez, ha még nincsenek legenerálva
        if (cachedRmfBottom.empty() || cachedRmfTop.empty() ||
            cachedRmfLeft.empty() || cachedRmfRight.empty()) {
            UpdateRMF();
        }

        // --- 1. Keresztirányú deriváltak (Normal Fence) kiszámítása az RMF projekcióval ---

        // A) S_v(u, 0) - Bottom határ menti kerítés (u mentén interpolálva)
        RMFFrame fB0 = GetFrameForHalfEdge(*bottom, cachedRmfBottom, 0.0f);
        RMFFrame fB1 = GetFrameForHalfEdge(*bottom, cachedRmfBottom, 1.0f);
        glm::vec3 velLeftStart  = -EvaluateHalfEdgeVelocity(*left, 1.0f);  // D0'(0)
        glm::vec3 velRightStart = EvaluateHalfEdgeVelocity(*right, 0.0f); // D1'(0)

        float alphaB0 = glm::dot(velLeftStart, fB0.t);
        float betaB0  = glm::dot(velLeftStart, fB0.s);
        float alphaB1 = glm::dot(velRightStart, fB1.t);
        float betaB1  = glm::dot(velRightStart, fB1.s);

        float alphaB = (1.0f - u) * alphaB0 + u * alphaB1;
        float betaB  = (1.0f - u) * betaB0  + u * betaB1;
        RMFFrame fB  = GetFrameForHalfEdge(*bottom, cachedRmfBottom, u);
        glm::vec3 Sv_u0 = alphaB * fB.t + betaB * fB.s;

        // B) S_v(u, 1) - Top határ menti kerítés (u mentén interpolálva)
        RMFFrame fT0 = GetFrameForHalfEdge(*top, cachedRmfTop, 0.0f);
        RMFFrame fT1 = GetFrameForHalfEdge(*top, cachedRmfTop, 1.0f);
        glm::vec3 velLeftEnd  = -EvaluateHalfEdgeVelocity(*right, 1.0f);  // D0'(1)
        glm::vec3 velRightEnd = EvaluateHalfEdgeVelocity(*left, 0.0f); // D1'(1)

        float alphaT0 = glm::dot(velLeftEnd, fT0.t);
        float betaT0  = glm::dot(velLeftEnd, fT0.s);
        float alphaT1 = glm::dot(velRightEnd, fT1.t);
        float betaT1  = glm::dot(velRightEnd, fT1.s);

        float alphaT = (1.0f - u) * alphaT0 + u * alphaT1;
        float betaT  = (1.0f - u) * betaT0  + u * betaT1;
        RMFFrame fT  = GetFrameForHalfEdge(*top, cachedRmfTop, u);
        glm::vec3 Sv_u1 = alphaT * fT.t + betaT * fT.s;

        // C) S_u(0, v) - Left határ menti kerítés (v mentén interpolálva)
        RMFFrame fL0 = GetFrameForHalfEdge(*left, cachedRmfLeft, 0.0f);
        RMFFrame fL1 = GetFrameForHalfEdge(*left, cachedRmfLeft, 1.0f);
        glm::vec3 velBottomStart = -EvaluateHalfEdgeVelocity(*top, 1.0f); // C0'(0)
        glm::vec3 velTopStart    = EvaluateHalfEdgeVelocity(*bottom, 0.0f);    // C1'(0)

        float alphaL0 = glm::dot(velBottomStart, fL0.t);
        float betaL0  = glm::dot(velBottomStart, fL0.s);
        float alphaL1 = glm::dot(velTopStart, fL1.t);
        float betaL1  = glm::dot(velTopStart, fL1.s);

        float alphaL = (1.0f - v) * alphaL0 + v * alphaL1;
        float betaL  = (1.0f - v) * betaL0  + v * betaL1;
        RMFFrame fL  = GetFrameForHalfEdge(*left, cachedRmfLeft, v);
        glm::vec3 Su_0v = alphaL * fL.t + betaL * fL.s;

        // D) S_u(1, v) - Right határ menti kerítés (v mentén interpolálva)
        RMFFrame fR0 = GetFrameForHalfEdge(*right, cachedRmfRight, 0.0f);
        RMFFrame fR1 = GetFrameForHalfEdge(*right, cachedRmfRight, 1.0f);
        glm::vec3 velBottomEnd = -EvaluateHalfEdgeVelocity(*bottom, 1.0f); // C0'(1)
        glm::vec3 velTopEnd    = EvaluateHalfEdgeVelocity(*top, 0.0f);    // C1'(1)

        float alphaR0 = glm::dot(velBottomEnd, fR0.t);
        float betaR0  = glm::dot(velBottomEnd, fR0.s);
        float alphaR1 = glm::dot(velTopEnd, fR1.t);
        float betaR1  = glm::dot(velTopEnd, fR1.s);

        float alphaR = (1.0f - v) * alphaR0 + v * alphaR1;
        float betaR  = (1.0f - v) * betaR0  + v * betaR1;
        RMFFrame fR  = GetFrameForHalfEdge(*right, cachedRmfRight, v);
        glm::vec3 Su_1v = alphaR * fR.t + betaR * fR.s;

        // --- 2. Hermite-szalagok kiszámítása ---

        // R1(u, v): u-irányú szalag
        glm::vec3 C0_u = bottom->EvaluatePosition(u);
        glm::vec3 C1_u = top->EvaluatePosition(1 - u);
        glm::vec3 R1 = C0_u * Hermite::F0(v) + C1_u * Hermite::F1(v) +
                       Sv_u0 * Hermite::G0(v) + Sv_u1 * Hermite::G1(v);

        // R2(u, v): v-irányú szalag
        glm::vec3 D0_v = left->EvaluatePosition(1 - v);
        glm::vec3 D1_v = right->EvaluatePosition(v);
        glm::vec3 R2 = D0_v * Hermite::F0(u) + D1_v * Hermite::F1(u) +
                       Su_0v * Hermite::G0(u) + Su_1v * Hermite::G1(u);

        // --- 3. Sarkok korrekciója (Corner Tensor Product R12) ---

        // Sarokpozíciók
        glm::vec3 P00 = bottom->EvaluatePosition(0.0f);
        glm::vec3 P10 = bottom->EvaluatePosition(1.0f);
        glm::vec3 P01 = top->EvaluatePosition(1.0f);
        glm::vec3 P11 = top->EvaluatePosition(0.0f);

        // Sarokderiváltak (Zero-twist feltételezés: S_uv = 0)
        glm::vec3 Sv00 = velLeftStart;   // S_v(0,0)
        glm::vec3 Sv10 = velRightStart;  // S_v(1,0)
        glm::vec3 Sv01 = velLeftEnd;     // S_v(0,1)
        glm::vec3 Sv11 = velRightEnd;    // S_v(1,1)

        glm::vec3 Su00 = velBottomStart; // S_u(0,0)
        glm::vec3 Su10 = velBottomEnd;   // S_u(1,0)
        glm::vec3 Su01 = velTopStart;    // S_u(0,1)
        glm::vec3 Su11 = velTopEnd;      // S_u(1,1)

        glm::vec3 R12 = Hermite::F0(u) * Hermite::F0(v) * P00  + Hermite::F0(u) * Hermite::F1(v) * P01  +
                        Hermite::F1(u) * Hermite::F0(v) * P10  + Hermite::F1(u) * Hermite::F1(v) * P11  +
                        Hermite::F0(u) * Hermite::G0(v) * Sv00 + Hermite::F0(u) * Hermite::G1(v) * Sv01 +
                        Hermite::F1(u) * Hermite::G0(v) * Sv10 + Hermite::F1(u) * Hermite::G1(v) * Sv11 +
                        Hermite::G0(u) * Hermite::F0(v) * Su00 + Hermite::G0(u) * Hermite::F1(v) * Su01 +
                        Hermite::G1(u) * Hermite::F0(v) * Su10 + Hermite::G1(u) * Hermite::F1(v) * Su11;

        // --- 4. Bikubikus Coons egyenlet összegzése ---
        return R1 + R2 - R12;
    }

    // Numerikus felületi normális kiszámítása (vektoriális szorzat a parciális deriváltakból)
    glm::vec3 EvaluateNormal(float u, float v) const override {
        const float eps = 0.001f;
        float u0 = glm::clamp(u - eps, 0.0f, 1.0f);
        float u1 = glm::clamp(u + eps, 0.0f, 1.0f);
        float v0 = glm::clamp(v - eps, 0.0f, 1.0f);
        float v1 = glm::clamp(v + eps, 0.0f, 1.0f);

        glm::vec3 dSdu = (Evaluate(u1, v) - Evaluate(u0, v)) / (u1 - u0);
        glm::vec3 dSdv = (Evaluate(u, v1) - Evaluate(u, v0)) / (v1 - v0);

        glm::vec3 normal = glm::cross(dSdu, dSdv);
        if (glm::length(normal) > 1e-6f) {
            return glm::normalize(normal);
        }
        return glm::vec3(0.0f, 1.0f, 0.0f);
    }

private:
    // Gyorsítótár (cache) az RMF keretekhez, hogy ne generálja újra 50 pontot minden Evaluate-nél
    mutable std::vector<RMFFrame> cachedRmfBottom;
    mutable std::vector<RMFFrame> cachedRmfTop;
    mutable std::vector<RMFFrame> cachedRmfLeft;
    mutable std::vector<RMFFrame> cachedRmfRight;
};