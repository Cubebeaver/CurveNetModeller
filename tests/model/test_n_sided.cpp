//
// Created by Cubebeaver on 2026. 11. 21..
//
#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/element/half_edge.h"
#include "model/element/n_sided_surface.hpp"
#include "model/element/point.h"
#include "model/element/line_segment.hpp"
#include "util/console_helper.hpp"


std::shared_ptr<NSidedSurface> ConstructSurface(int N = 5) {
    std::vector<std::shared_ptr<Point>> points;
    for (int i = 0; i < N; i++) {
        float t = static_cast<float>(i) / static_cast<float>(N);
        float angle = 2 * std::numbers::pi_v<float> * t;

        points.push_back(std::make_shared<Point>(std::cos(angle), 0, std::sin(angle)));
    }


    std::vector<std::shared_ptr<ICurve>> edges;
    for (int i = 0; i < N; i++) {
        edges.push_back(std::make_shared<LineSegment>(points[i], points[(i + 1) % N]));
    }

    std::vector<std::shared_ptr<HalfEdge>> halfEdges;
    for (int i = 0; i < N; i++) {
        halfEdges.push_back(std::make_shared<HalfEdge>(edges[i]));
    }

    std::shared_ptr<NSidedSurface> surface = std::make_shared<NSidedSurface>(halfEdges);
    return surface;
}

TEST(NSidedTest, GlobalToLocalParameterSpace) {
    auto surface = ConstructSurface(5);

    int res = 20;
    for (int v = res; v >= 0; v--) {
        for (int u = 0; u <= res; u++) {
            if (surface->IsInDomain(static_cast<float>(u) / res, static_cast<float>(v) / res))
                std::cout << "#";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    // Előző szomszéd elején
    auto loc1 = surface->GetLocalParameter(2, 0.654508f, 0.975528f);
    EXPECT_NEAR(loc1.s, 0.0f, 0.001f);
    EXPECT_NEAR(loc1.d, 1.0f, 0.001f);

    // Szembe csúcs
    auto loc2 = surface->GetLocalParameter(2, 1.0f, 0.5f);
    EXPECT_NEAR(loc2.s, 0.5f, 0.001f);
    EXPECT_NEAR(loc2.d, 1.0f, 0.001f);

    // Saját sarok
    auto loc3 = surface->GetLocalParameter(2, 0.0954916f, 0.206107f);
    EXPECT_NEAR(loc3.s, 1.0f, 0.001f);
    EXPECT_NEAR(loc3.d, 0.0f, 0.001f);

    // Saját közép
    auto loc4 = surface->GetLocalParameter(2, 0.0954916f, 0.5f);
    EXPECT_NEAR(loc4.s, 0.5f, 0.001f);
    EXPECT_NEAR(loc4.d, 0.0f, 0.001f);

    // Domén közepe
    auto loc5 = surface->GetLocalParameter(2, 0.5f, 0.5f);
    EXPECT_NEAR(loc5.s, 0.5f, 0.001f);
    EXPECT_NEAR(loc5.d, (0.5 - 0.0954916f) / (1 - 0.0954916f), 0.001f);

    // Előző él közepe
    auto loc6 = surface->GetLocalParameter(2, (0.654508f + 0.0954915) / 2, (0.975528f + 0.793893f) / 2);
    EXPECT_NEAR(loc6.s, 0.0f, 0.001f);
    EXPECT_NEAR(loc6.d, 0.5f, 0.001f);
}

TEST(NSidedTest, CenterPointParameterEquality) {
    auto surface = ConstructSurface(5);

    LocalParameter lps[5];
    for (int i = 0; i < 5; i++) {
        lps[i] = surface->GetLocalParameter(i, 0.5f, 0.5f);
    }

    for (int i = 0; i < 5; i++) {
        std::cout << "---------------------------------------------- " << i << std::endl;
        EXPECT_NEAR(lps[i].s, lps[(i + 1) % 5].s, 0.001f);
        EXPECT_NEAR(lps[i].d, lps[(i + 1) % 5].d, 0.001f);
    }
}

TEST(NSidedTest, Compile) {
    auto surface = ConstructSurface(5);
    surface->Evaluate(0.5f, 0.5f);
}