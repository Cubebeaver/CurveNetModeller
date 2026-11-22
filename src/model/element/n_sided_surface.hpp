#pragma once

#include <numbers>
#include <optional>
#include <glm/glm.hpp>

#include "half_edge.h"
#include "i_Surface.hpp"

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
public:
    std::vector<std::shared_ptr<HalfEdge>> edges;

    NSidedSurface() { }
    NSidedSurface(std::vector<std::shared_ptr<HalfEdge>> edges)
                : edges(edges) {
        if (edges.size() <= 3) throw std::length_error("NSidedSurface has to have at least 4 edges");
        UpdateDomainVertices();
        // Events
    }

    void InitializeAfterLoad() override { /*Events*/ UpdateDomainVertices(); }

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
        if (EdgeCount() == 4) return LocalParameter(u, v); //! Ez így nem jó!!! El kell forgatniii!!!
        glm::vec2 point(u, v);

        // A mostani, és a jobb/bal szomszédok végpontjai (igaz ezekből 2 felesleges, mert egybeesnek, but oh well...)
        EdgeEnds currEnds  = GetDomainEdgeVerticesInParameterSpace(edgeIdx);
        EdgeEnds nextNbour = GetDomainEdgeVerticesInParameterSpace(NextIndex(edgeIdx));
        EdgeEnds prevNbour = GetDomainEdgeVerticesInParameterSpace(PrevIndex(edgeIdx));

        std::cout << "Should be 1/true" << (currEnds.start == prevNbour.end) << std::endl;
        std::cout << "Should be 1/true" << (currEnds.end == nextNbour.start) << std::endl;

        std::cout << currEnds.start.x  << " currEnds.start "  << currEnds.start.y << std::endl;
        std::cout << currEnds.end.x    << " currEnds.end "    << currEnds.end.y   << std::endl;
        std::cout << prevNbour.start.x << " prevNbour.start " << prevNbour.start.y << std::endl;
        std::cout << prevNbour.end.x   << " prevNbour.end "   << prevNbour.end.y   << std::endl;
        std::cout << nextNbour.start.x << " nextNbour.start " << nextNbour.start.y << std::endl;
        std::cout << nextNbour.end.x   << " nextNbour.end "   << nextNbour.end.y   << std::endl;

        // A szomszéd-egyenesek metszete, amihez képest a lokális s és d paramétereket számoljuk
        glm::vec2 distanceAnchor = getLineIntersection(nextNbour.start, nextNbour.end - nextNbour.start,
                                                       prevNbour.start, prevNbour.end - prevNbour.start);

        std::cout << distanceAnchor.x << " da " << distanceAnchor.y << std::endl;

        // A jelenlegi (=kiválasztott) egyenes és a distanceAnchor->point vektor
        glm::vec2 distanceVector(point - distanceAnchor);
        glm::vec2 currectEdgeIntersection = getLineIntersection(currEnds.start, currEnds.end - currEnds.start,
                                                                distanceAnchor, distanceVector);

        std::cout << distanceVector.x << " dv " << distanceVector.y << std::endl;
        std::cout << currectEdgeIntersection.x << " ce-dv-i " << currectEdgeIntersection.y << std::endl;

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

        std::optional<glm::vec2> oppositeIntersection;
        int nextNextIdx = NextIndex(NextIndex(edgeIdx));
        for (int i = 0; i < EdgeCount() - 3; i++) {
            EdgeEnds ee = GetDomainEdgeVerticesInParameterSpace(nextNextIdx);
            auto intersection = getSegmentLineIntersection(
                ee.start, ee.end,                      // Segment
                distanceAnchor, point - distanceAnchor // Line
            );
            if (intersection.has_value()) {
                oppositeIntersection = intersection.value();
                break;
            }

            nextNextIdx = NextIndex(nextNextIdx);
        }
        if (!oppositeIntersection.has_value()) {
            std::cout << "parameter outside of parameter space" << std::endl;
        }

        float maxDistance = glm::length(oppositeIntersection.value() - currectEdgeIntersection);
        d = d / maxDistance;

        // Végre vége
        return {s, d};
    }

    glm::vec3 Evaluate(float u, float v) const override {
        return glm::vec3(0, 0, 0);
    }



private:
    std::vector<glm::vec2> cachedDomainVertices;

    void UpdateDomainVertices() {
        constexpr float PI = std::numbers::pi_v<float>;
        int N = EdgeCount();

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

    void OnHalfEdgeChanged() {
        SurfaceChanged.Invoke();
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
            return glm::vec2(0, 0);
        }

        glm::vec2 p21 = p2 - p1;
        float t = cross2D(p21, v2) / denom;

        return p1 + t * v1;
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
};





