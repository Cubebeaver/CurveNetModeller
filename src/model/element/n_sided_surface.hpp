#pragma once

#include <numbers>
#include <optional>
#include <glm/glm.hpp>

#include "half_edge.h"
#include "i_Surface.hpp"
#include "rotation_minimizing_frame.hpp"

#include "util/math_helper.hpp"

struct LocalParameter {
    float s;
    float d;

    LocalParameter() = default;
    LocalParameter(float s, float d) : s(s), d(d) { }
};

struct EdgeEnds {
    glm::vec2 start;
    glm::vec2 end;

    EdgeEnds(glm::vec2 start, glm::vec2 end) : start(start), end(end) { }
};

class NSidedSurface : public ISurface {
    glm::vec3 debugStartNormal = glm::vec3(0, 1, 0);
    glm::vec3 debugEndNormal = glm::vec3(0, 1, 0);

public:
    std::vector<std::shared_ptr<HalfEdge>> edges;
    int rmfResolution = 50;

    NSidedSurface() { }
    NSidedSurface(std::vector<std::shared_ptr<HalfEdge>> edges)
                : edges(edges) {
        if (edges.size() <= 3) throw std::length_error("NSidedSurface has to have at least 4 edges");
        UpdateDomainVertices();
        UpdateAllRMFs();
        // Events
    }

    void InitializeAfterLoad() override {
        // Events
        UpdateDomainVertices();
        UpdateAllRMFs();
    }

    virtual ~NSidedSurface() override { /*Unsubscribe from events*/ }

    int EdgeCount() const {
        return edges.size();
    }

    int NextIndex(int i) const {
        if (i < EdgeCount() - 1) return i + 1;
        else /*    if last    */ return 0;
    }

    int PrevIndex(int i) const {
        if (i >= 1)         return i - 1;
        else /* if first */ return EdgeCount() - 1;
    }

    bool IsInDomain(float u, float v) const {
        size_t n = cachedDomainVertices.size();
        glm::vec2 point(u, v);

        for (size_t i = 0; i < n; ++i) {
            glm::vec2 a = cachedDomainVertices[i];
            glm::vec2 b = cachedDomainVertices[(i + 1) % n]; // Következő csúcs (az utolsó után az első)

            glm::vec2 edge = b - a;
            glm::vec2 toPoint = point - a;

            // Ha a kereszttermék negatív, a pont az éltől jobbra van -> kívül esik
            if (cross2D(edge, toPoint) < 0.0f) {
                return false;
            }
        }

        return true; // Ha minden élnek a bal oldalán van, a sokszögön belül van
    }

    EdgeEnds GetDomainEdgeVerticesInParameterSpace(int edgeIdx) const {
        auto start = cachedDomainVertices[ edgeIdx];
        auto end   = cachedDomainVertices[(edgeIdx + 1) % EdgeCount()];

        return EdgeEnds(start, end);
    }

    LocalParameter GetLocalParameter(int edgeIdx, float u, float v) const {
        // Ha négyzet alakú, akkor a páruzamos egyenesek miatt nem tudnánk metszetet találni
        const int N = EdgeCount();
        if (N == 4) return LocalParameter(u, v); //! Ez így nem jó!!! El kell forgatniii!!!
        glm::vec2 point(u, v);

        // A mostani, és a jobb/bal szomszédok végpontjai (igaz ezekből 2 felesleges, mert egybeesnek, but oh well...)
        EdgeEnds currEnds  = GetDomainEdgeVerticesInParameterSpace(edgeIdx);
        EdgeEnds nextNbour = GetDomainEdgeVerticesInParameterSpace(NextIndex(edgeIdx));
        EdgeEnds prevNbour = GetDomainEdgeVerticesInParameterSpace(PrevIndex(edgeIdx));

        // A szomszéd-egyenesek metszete, amihez képest a lokális s és d paramétereket számoljuk
        glm::vec2 distanceAnchor = getLineIntersection(nextNbour.start, nextNbour.end - nextNbour.start,
                                                       prevNbour.start, prevNbour.end - prevNbour.start);

        // A jelenlegi (=kiválasztott) egyenes és a distanceAnchor->point vektor
        glm::vec2 distanceVector(point - distanceAnchor);
        glm::vec2 currectEdgeIntersection = getLineIntersection(currEnds.start, currEnds.end - currEnds.start,
                                                                distanceAnchor, distanceVector);

        // Lokális side paraméter
        auto sa_0 = currectEdgeIntersection - currEnds.start; // Actual distance that we care about
        auto sr_1 = currEnds.end - currEnds.start; // Reference vector for determining sign
        float s = std::copysign(glm::length(sa_0), glm::dot(sa_0, sr_1));

        // Lokális distance paraméter
        auto da_0 = point - currectEdgeIntersection; // Actual distance that we care about
        auto dr_1 = point - distanceAnchor; // Reference vector for determining sign
        float d = std::copysign(glm::length(da_0), glm::dot(da_0, dr_1));

        // Skálázzuk a lokális teret hogy ne a "globális" paraméter térben legyenek a távolságok
        float paramSpaceSideLength = glm::length(currEnds.end - currEnds.start);
        s = s / paramSpaceSideLength;
        // Skálázzuk a d-t hogy a paraméter térben a szomszédos élek vége a d=1-nek feleljen meg?????? de ez lehet full rossz elképzelés
        // Lehet hogy a distanceAnchor miatti """távolág-tér"""--"""ívesség""" miatt a körív tetejének kellene a szemközti csúccsal egybeesni????
        // Passz, de sztem nem, jó ahogy most van
        // TODO kitalálni
        // DE!! ez lesz a jó!! a Diában is a szomszédos élek távoli pontjával esik egybe
        // Ebben az esetben hogy minden élnek egyenlő a hossza a paraméter-térben, ez jó is,
        // de ha nem lesznek egyenlő hosszúak, akkor s alapján lineárisan kell sálázni a prev, és a nex között.
        // Most hogy egyenlőek, lehetne alanitikusan számolni ezt a paramSpaceSideLength-et, de amúgy nem.
        // Lehet mégse jó, mert nemhiszem hogy az "távolság-ívre" esik a d=1
        // TODO utánaszámolni
        // Ne de most sztem ez így tényleg jó, az én matekom alapján biztos,
        // de csak az egyenlő esetre, de sztem a rendes implementáció idő híjján gatya
        //! MEGVAN:
        // A domén másik oldalán kell lennie, ami szögletes, ami meg furcsa,
        // de végülis ez a logikus, hogy a domén teljes területén 0 és 1 közé esnen a lokál paraméterek

        glm::vec2 oppositeIntersection;
        int nextNextIdx = NextIndex(NextIndex(edgeIdx));
        float minDistance = std::numeric_limits<float>::infinity();
        for (int i = 0; i < N - 3; i++) {
            EdgeEnds ee = GetDomainEdgeVerticesInParameterSpace(nextNextIdx);
            auto intersection = getLineIntersectionPoints(
                ee.start, ee.end,
                distanceAnchor, point - distanceAnchor
            );
            if (std::isnan(ee.start.x)) continue;

            float currDst = glm::length(intersection - distanceAnchor);
            if (currDst < minDistance) {
                oppositeIntersection = intersection;
                minDistance = currDst;
            }

            nextNextIdx = NextIndex(nextNextIdx);
        }

        d = d / minDistance;

        // Végre vége
        return {s, d};
    }

    glm::vec3 GetEdgeCrossDerivate(int edgeIdx, float s) const {
        RMFFrame f0 = RMF::Sample(cachedRMFs[edgeIdx], 0.0f);
        RMFFrame f1 = RMF::Sample(cachedRMFs[edgeIdx], 1.0f);
        glm::vec3 nbourVel0 = -edges[PrevIndex(edgeIdx)]->EvaluateVelocity(1.0f);
        glm::vec3 nbourVel1 =  edges[NextIndex(edgeIdx)]->EvaluateVelocity(0.0f);

        float alphaB0 = glm::dot(nbourVel0, f0.t);
        float betaB0  = glm::dot(nbourVel0, f0.s);
        //float gammaB0 = glm::dot(nbourVel0, f0.r);

        float alphaB1 = glm::dot(nbourVel1, f1.t);
        float betaB1  = glm::dot(nbourVel1, f1.s);
        //float gammaB1 = glm::dot(nbourVel1, f1.r);

        float alphaB = (1.0f - s) * alphaB0 + s * alphaB1;
        float betaB  = (1.0f - s) * betaB0  + s * betaB1;
        //float gammaB = (1.0f - s) * gammaB0 + s * gammaB1

        RMFFrame f = RMF::Sample(cachedRMFs[edgeIdx], s);
        glm::vec3 binormal = alphaB * f.t + betaB * f.s /* + gammaB * f.r*/;
        return binormal;
    }

    glm::vec3 EveluateRibbonLocal(int edgeIdx, float s, float t) const {
        glm::vec3 s_t = edges[edgeIdx]->EvaluatePosition(s);
        glm::vec3 b_t = GetEdgeCrossDerivate(edgeIdx, s);

        return s_t + t * b_t;
    }

    float GetBlendDistance(int edgeIdx, float u, float v) const {
        glm::vec2 point(u, v);
        EdgeEnds ends = GetDomainEdgeVerticesInParameterSpace(edgeIdx);

        float distance = distancePointToLine2DPoints(point, ends.start, ends.end);
        return distance;
    }

    std::vector<float> GetBlendWeights(float u, float v) const {
        const int N = EdgeCount();

        std::vector<float> d(N, 0.0f);
        for (int i = 0; i < N; i++) {
            d[i] = GetBlendDistance(i, u, v);
        }

        std::vector<float> weights(N, 0.0f);
        const float EPSILON = 1e-6f;

        // 1. Ellenőrzés: Közel vagyunk-e valamelyik élhez?
        for (int i = 0; i < N; ++i) {
            if (d[i] < EPSILON) {
                // Ha rajta vagyunk az i-edik élen, a súlya 1, a többi 0
                weights[i] = 1.0f;
                return weights;
            }
        }

        // 2. Inverz súlyok összegzése
        float sumInverse = 0.0f;
        for (int i = 0; i < N; ++i) {
            float invD2 = 1.0f / (d[i] * d[i]);
            weights[i] = invD2;
            sumInverse += invD2;
        }

        // 3. Normalizálás
        for (int i = 0; i < N; ++i) {
            weights[i] /= sumInverse;
        }

        return weights;
    }

    glm::vec3 Evaluate(float u, float v) const override {
        const int N = EdgeCount();

        glm::vec3 sum(0, 0, 0);

        auto blendWeights = GetBlendWeights(u, v);

        for (int i = 0; i < N; i++) {
            LocalParameter local = GetLocalParameter(i, u, v);
            sum += EveluateRibbonLocal(i, local.s, local.d) * blendWeights[i];
        }

        return sum;
    }

    // 2D kereszttermék (determináns): a.x * b.y - a.y * b.x
    inline static float cross2D(const glm::vec2& a, const glm::vec2& b) {
        return a.x * b.y - a.y * b.x;
    }

    // Végtelen egyenesek metszéspontja
    inline static glm::vec2 getLineIntersection(
        glm::vec2 p1, glm::vec2 v1, // 1. egyenes: pont és irányvektor
        glm::vec2 p2, glm::vec2 v2  // 2. egyenes: pont és irányvektor
    ) {
        float denom = cross2D(v1, v2);

        // Ha a determináns ~0, az egyenesek párhuzamosak (vagy egybeesnek)
        if (std::abs(denom) < 1e-6f) {
            std::cout << "Could not find intersection" << std::endl;
            return glm::vec2(std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN());
        }

        glm::vec2 p21 = p2 - p1;
        float t = cross2D(p21, v2) / denom;

        return p1 + t * v1;
    }

    inline static glm::vec2 getLineIntersectionPoints(
        glm::vec2 a1, glm::vec2 b1, // 1. egyenes: pont és pont
        glm::vec2 a2, glm::vec2 b2  // 2. egyenes: pont és pont
    ) {
        return getLineIntersection(a1, b1 - a1, a2, b2 - a2);
    }

    inline static std::optional<glm::vec2> getSegmentLineIntersection(
        glm::vec2 seg_a,  glm::vec2 seg_b,  // Szakasz végpontjai
        glm::vec2 line_p, glm::vec2 line_v, // Egyenes két pontja (C és D)
        float epsilon = 1e-5f               // floating point hibák miatt egy kis engedékenység
    ) {
        glm::vec2 v1 = seg_b - seg_a; // Szakasz irányvektora
        glm::vec2 v2 = line_v;        // Egyenes irányvektora

        float denom = cross2D(v1, v2);

        // Ha ~0, a szakasz és az egyenes párhuzamosak (vagy egybeesnek)
        if (std::abs(denom) < 1e-6f) {
            return std::nullopt;
        }

        glm::vec2 p21 = line_p - seg_a;

        // Csak a szakasz (v1) 't' paraméterét számoljuk ki:
        float t = cross2D(p21, v2) / denom;

        // Ha t 0 és 1 közé esik, a metszéspont a szakaszon BELÜL van
        // (Az egyenes paraméterét nem kell vizsgálni, mert az végtelen)
        if (t >= 0.0f - epsilon && t <= 1.0f + epsilon) {
            return seg_a + t * v1;
        }

        return std::nullopt; // Az egyenes metszéspontja a szakasz vonalán kívülre esne
    }

    // Távolság egy pont és egy egyenes között, ahol az egyenes egy pont és egy irányvektorral van megadva
    static float distancePointToLine2D(const glm::vec2& point, const glm::vec2& linePoint, const glm::vec2& lineDir) {
        auto normDir = glm::normalize(lineDir);

        glm::vec2 normal(-normDir.y, normDir.x);
        return std::abs(glm::dot(point - linePoint, normal));
    }

    // Távolság egy pont és egy egyenes között, ahol az egyenes két pontjával van megadva
    static float distancePointToLine2DPoints(const glm::vec2& point, const glm::vec2& lineA, const glm::vec2& lineB) {
        glm::vec2 AB = lineB - lineA;
        glm::vec2 normal(-AB.y, AB.x);

        float len = glm::length(AB);
        if (len == 0.0f) return glm::distance(point, lineA);

        return (glm::dot(point - lineA, normal)) / len;
    }

    const std::vector<glm::vec2>& GetDomainVertices() const { return cachedDomainVertices; }

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(edges), CEREAL_NVP(rmfResolution));
    }

private:
    std::vector<glm::vec2> cachedDomainVertices;
    std::vector<std::vector<RMFFrame>> cachedRMFs;


    void UpdateDomainVertices() {
        constexpr float PI = std::numbers::pi_v<float>;
        const int N = EdgeCount();

        cachedDomainVertices.clear();
        cachedDomainVertices.reserve(N);
        for (int i = 0; i < N; i++) {
            float t = (2 * PI) * (static_cast<float>(i) / N);
            float u = std::cos(t);
            float v = std::sin(t);
            auto from0to1 = glm::vec2((u + 1) / 2, (v + 1) / 2);
            cachedDomainVertices.push_back(from0to1);
            std::cout << from0to1.x << " psv " << from0to1.y << std::endl;
        }
    }

    void UpdateRMF(int edgeIdx) {
        cachedRMFs[edgeIdx] = RMF::GenerateFrames(edges[edgeIdx], debugStartNormal, debugEndNormal, rmfResolution);
    }

    void UpdateAllRMFs() {
        cachedRMFs.clear();
        cachedRMFs.resize(EdgeCount());

        for (int i = 0; i < EdgeCount(); i++) {
            UpdateRMF(i);
        }
    }

    void OnHalfEdgeChanged() {
        SurfaceChanged.Invoke();
    }
};
