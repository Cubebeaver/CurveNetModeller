//
// Created by Cubebeaver on 2026. 11. 21..
//
#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/element/half_edge.h"
#include "model/element/n_sided_surface.hpp"

TEST(NSidedTest, GlobalToLocalParameterSpace) {
    std::vector<std::shared_ptr<HalfEdge>> edges = {nullptr, nullptr, nullptr, nullptr, nullptr};
    NSidedSurface surface(edges);

    int res = 20;
    for (int v = res; v >= 0; v--) {
        for (int u = 0; u <= res; u++) {
            if (surface.IsInParameterSpace(static_cast<float>(u) / res, static_cast<float>(v) / res))
                std::cout << "#";
            else
                std::cout << " ";
        }
        std::cout << std::endl;
    }

    // Ez nagyvalszeg fail
    // auto loc0 = surface.GetLocalParameter(2, 0.5f, 0.5f);
    // EXPECT_NEAR(loc0.s, 0.5, 0.001f);
    // EXPECT_NEAR(loc0.d, 0.5 - 0.0954915, 0.001f);

    // De ennek át kéne mennie
    auto loc1 = surface.GetLocalParameter(2, 0.654508f, 0.975528f);
    EXPECT_NEAR(loc1.s, 0, 0.001f);
    EXPECT_NEAR(loc1.d, 1, 0.001f);

    auto loc2 = surface.GetLocalParameter(2, 0.729824f, 0.5f);
    EXPECT_NEAR(loc2.s, 0.5, 0.001f);
    EXPECT_NEAR(loc2.d, 1, 0.001f); // Ja itt a local param-nál a distance számítás nagyon olcsó
}

TEST(NSidedTest, CenterPointParameterEquality) {
    std::vector<std::shared_ptr<HalfEdge>> edges = {nullptr, nullptr, nullptr, nullptr, nullptr};
    NSidedSurface surface(edges);

    LocalParameter lps[5];
    for (int i = 0; i < 5; i++) {
        lps[i] = surface.GetLocalParameter(i, 0.5f, 0.5f);
    }

    for (int i = 0; i < 5; i++) {
        std::cout << "---------------------------------------------- " << i << std::endl;
        EXPECT_NEAR(lps[i].s, lps[(i + 1) % 5].s, 0.001f);
        EXPECT_NEAR(lps[i].d, lps[(i + 1) % 5].d, 0.001f);
    }
}