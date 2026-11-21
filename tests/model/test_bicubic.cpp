#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/element/bezier_curve.h"
#include "model/element/bezier_node.h"
#include "model/element/bicubic_coons_new.hpp"


TEST(BicubicCoonsSurfaceTest, CornerEquality) {
    std::shared_ptr<BezierCurve> c1 = std::make_shared<BezierCurve>();
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-3, -1, -2), glm::vec3(-1,  1, -2)));
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 1,  1, -2), glm::vec3( 3, -1, -2)));
    std::shared_ptr<BezierCurve> c2 = std::make_shared<BezierCurve>();
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-3, -1,  2), glm::vec3(-1,  1,  2)));
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 1,  1,  2), glm::vec3( 3, -1,  2)));
    std::shared_ptr<BezierCurve> d1 = std::make_shared<BezierCurve>();
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-2,  1, -3), glm::vec3(-2, -1, -1)));
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-2, -1,  1), glm::vec3(-2,  1,  3)));
    std::shared_ptr<BezierCurve> d2 = std::make_shared<BezierCurve>();
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 2,  1, -3), glm::vec3( 2, -1, -1)));
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 2, -1,  1), glm::vec3( 2,  1,  3)));

    auto c1_he = std::make_shared<HalfEdge>(c1);
    auto c2_he = std::make_shared<HalfEdge>(c2, true);
    auto d1_he = std::make_shared<HalfEdge>(d1, true);
    auto d2_he = std::make_shared<HalfEdge>(d2);

    BicubicCoonsSurface coons(c1_he, c2_he, d1_he, d2_he);

    // Bottom left
    EXPECT_NEAR(coons.S_u0(0).x, coons.S_0v(0).x, 0.001f);
    EXPECT_NEAR(coons.S_u0(0).y, coons.S_0v(0).y, 0.001f);
    EXPECT_NEAR(coons.S_u0(0).z, coons.S_0v(0).z, 0.001f);

    // Bottom right
    EXPECT_NEAR(coons.S_u0(1).x, coons.S_1v(0).x, 0.001f);
    EXPECT_NEAR(coons.S_u0(1).y, coons.S_1v(0).y, 0.001f);
    EXPECT_NEAR(coons.S_u0(1).z, coons.S_1v(0).z, 0.001f);

    // Top left
    EXPECT_NEAR(coons.S_u1(0).x, coons.S_0v(1).x, 0.001f);
    EXPECT_NEAR(coons.S_u1(0).y, coons.S_0v(1).y, 0.001f);
    EXPECT_NEAR(coons.S_u1(0).z, coons.S_0v(1).z, 0.001f);

    // Top right
    EXPECT_NEAR(coons.S_u1(1).x, coons.S_1v(1).x, 0.001f);
    EXPECT_NEAR(coons.S_u1(1).y, coons.S_1v(1).y, 0.001f);
    EXPECT_NEAR(coons.S_u1(1).z, coons.S_1v(1).z, 0.001f);
}


TEST(BicubicCoonsSurfaceTest, BoundaryEdgeEquality) {
    std::shared_ptr<BezierCurve> c1 = std::make_shared<BezierCurve>();
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-3, -1, -2), glm::vec3(-1,  1, -2)));
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 1,  1, -2), glm::vec3( 3, -1, -2)));
    std::shared_ptr<BezierCurve> c2 = std::make_shared<BezierCurve>();
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-3, -1,  2), glm::vec3(-1,  1,  2)));
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 1,  1,  2), glm::vec3( 3, -1,  2)));
    std::shared_ptr<BezierCurve> d1 = std::make_shared<BezierCurve>();
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-2,  1, -3), glm::vec3(-2, -1, -1)));
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-2, -1,  1), glm::vec3(-2,  1,  3)));
    std::shared_ptr<BezierCurve> d2 = std::make_shared<BezierCurve>();
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 2,  1, -3), glm::vec3( 2, -1, -1)));
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 2, -1,  1), glm::vec3( 2,  1,  3)));

    auto c1_he = std::make_shared<HalfEdge>(c1);
    auto c2_he = std::make_shared<HalfEdge>(c2, true);
    auto d1_he = std::make_shared<HalfEdge>(d1, true);
    auto d2_he = std::make_shared<HalfEdge>(d2);

    BicubicCoonsSurface coons(c1_he, c2_he, d1_he, d2_he);

    float ts[] = {0.0f, 0.1f, 0.25f, 0.333f, 0.5f, 1.0f};

    for (auto t : ts) {
        // Bottom / C1
        EXPECT_NEAR(coons.S_u0(t).x, c1->EvaluatePosition(t).x, 0.001f);
        EXPECT_NEAR(coons.S_u0(t).y, c1->EvaluatePosition(t).y, 0.001f);
        EXPECT_NEAR(coons.S_u0(t).z, c1->EvaluatePosition(t).z, 0.001f);

        // Top / C2
        EXPECT_NEAR(coons.S_u1(t).x, c2->EvaluatePosition(t).x, 0.001f);
        EXPECT_NEAR(coons.S_u1(t).y, c2->EvaluatePosition(t).y, 0.001f);
        EXPECT_NEAR(coons.S_u1(t).z, c2->EvaluatePosition(t).z, 0.001f);

        // Left / D1
        EXPECT_NEAR(coons.S_0v(t).x, d1->EvaluatePosition(t).x, 0.001f);
        EXPECT_NEAR(coons.S_0v(t).y, d1->EvaluatePosition(t).y, 0.001f);
        EXPECT_NEAR(coons.S_0v(t).z, d1->EvaluatePosition(t).z, 0.001f);

        // RIGHT / D2
        EXPECT_NEAR(coons.S_1v(t).x, d2->EvaluatePosition(t).x, 0.001f);
        EXPECT_NEAR(coons.S_1v(t).y, d2->EvaluatePosition(t).y, 0.001f);
        EXPECT_NEAR(coons.S_1v(t).z, d2->EvaluatePosition(t).z, 0.001f);
    }
}


TEST(BicubicCoonsSurfaceTest, SurfaceBoundaryEquality) {
    std::shared_ptr<BezierCurve> c1 = std::make_shared<BezierCurve>();
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-3, -1, -2), glm::vec3(-1,  1, -2)));
    c1->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 1,  1, -2), glm::vec3( 3, -1, -2)));
    std::shared_ptr<BezierCurve> c2 = std::make_shared<BezierCurve>();
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-3, -1,  2), glm::vec3(-1,  1,  2)));
    c2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 1,  1,  2), glm::vec3( 3, -1,  2)));
    std::shared_ptr<BezierCurve> d1 = std::make_shared<BezierCurve>();
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0, -2), glm::vec3(-2,  1, -3), glm::vec3(-2, -1, -1)));
    d1->AddNode(std::make_shared<BezierNode>(glm::vec3(-2,  0,  2), glm::vec3(-2, -1,  1), glm::vec3(-2,  1,  3)));
    std::shared_ptr<BezierCurve> d2 = std::make_shared<BezierCurve>();
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0, -2), glm::vec3( 2,  1, -3), glm::vec3( 2, -1, -1)));
    d2->AddNode(std::make_shared<BezierNode>(glm::vec3( 2,  0,  2), glm::vec3( 2, -1,  1), glm::vec3( 2,  1,  3)));

    auto c1_he = std::make_shared<HalfEdge>(c1);
    auto c2_he = std::make_shared<HalfEdge>(c2, true);
    auto d1_he = std::make_shared<HalfEdge>(d1, true);
    auto d2_he = std::make_shared<HalfEdge>(d2);

    BicubicCoonsSurface coons(c1_he, c2_he, d1_he, d2_he);

    float ts[] = {0.0f, 0.1f, 0.25f, 0.333f, 0.5f, 1.0f};

    for (auto t : ts) {
        // Bottom / C1
        EXPECT_NEAR(coons.Evaluate(t, 0).x, c1->EvaluatePosition(t).x, 0.01f);
        EXPECT_NEAR(coons.Evaluate(t, 0).y, c1->EvaluatePosition(t).y, 0.01f);
        EXPECT_NEAR(coons.Evaluate(t, 0).z, c1->EvaluatePosition(t).z, 0.01f);

        // Top / C2
        EXPECT_NEAR(coons.Evaluate(t, 1).x, c2->EvaluatePosition(t).x, 0.01f);
        EXPECT_NEAR(coons.Evaluate(t, 1).y, c2->EvaluatePosition(t).y, 0.01f);
        EXPECT_NEAR(coons.Evaluate(t, 1).z, c2->EvaluatePosition(t).z, 0.01f);

        // Left / D1
        EXPECT_NEAR(coons.Evaluate(0, t).x, d1->EvaluatePosition(t).x, 0.01f);
        EXPECT_NEAR(coons.Evaluate(0, t).y, d1->EvaluatePosition(t).y, 0.01f);
        EXPECT_NEAR(coons.Evaluate(0, t).z, d1->EvaluatePosition(t).z, 0.01f);

        // RIGHT / D2
        EXPECT_NEAR(coons.Evaluate(1, t).x, d2->EvaluatePosition(t).x, 0.01f);
        EXPECT_NEAR(coons.Evaluate(1, t).y, d2->EvaluatePosition(t).y, 0.01f);
        EXPECT_NEAR(coons.Evaluate(1, t).z, d2->EvaluatePosition(t).z, 0.01f);
    }
}

