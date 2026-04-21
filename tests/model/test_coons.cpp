#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "../../src/model/element/bezier_curve.h"
#include "../../src/model/element/bezier_node.h"
#include "../../src/model/element/coons_surface.h"


TEST(CoonsSurfaceTest, EvaluatePoint) {
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

    CoonsSurface coons(c1, c2, d1, d2);

    auto pos_center = coons.Evaluate(0.5f, 0.5f);
    EXPECT_NEAR(pos_center.x, 0.0f, 0.001);
    EXPECT_NEAR(pos_center.y, 0.0f, 0.001);
    EXPECT_NEAR(pos_center.z, 0.0f, 0.001);

    auto pos_corner = coons.Evaluate(0.0f, 0.0f);
    EXPECT_NEAR(pos_corner.x, -2.0f, 0.001);
    EXPECT_NEAR(pos_corner.y,  0.0f, 0.001);
    EXPECT_NEAR(pos_corner.z, -2.0f, 0.001);

    auto pos_random = coons.Evaluate(0.25f, 0.75f);
    EXPECT_NEAR(pos_random.x, -1.09375f, 0.001);
    EXPECT_NEAR(pos_random.y,  0.0f, 0.001);
    EXPECT_NEAR(pos_random.z,  1.09375f, 0.001);
}


TEST(CoonsSurfaceTest, EvaluateNormal) {
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

    CoonsSurface coons(c1, c2, d1, d2);

    auto norm_center = coons.EvaluateNormal(0.5f, 0.5f);
    EXPECT_NEAR(norm_center.x, 0.0f, 0.01);
    EXPECT_NEAR(norm_center.y, 1.0f, 0.01);
    EXPECT_NEAR(norm_center.z, 0.0f, 0.01);

    auto norm_corner = coons.EvaluateNormal(0.0f, 0.0f);
    EXPECT_NEAR(norm_corner.x, -0.5735f, 0.01);
    EXPECT_NEAR(norm_corner.y,  0.5850f, 0.01);
    EXPECT_NEAR(norm_corner.z,  0.5735f, 0.01);

    auto norm_random = coons.EvaluateNormal(0.25f, 0.75f);
    EXPECT_NEAR(norm_random.x, -0.3156f, 0.01);
    EXPECT_NEAR(norm_random.y,  0.8892f, 0.01);
    EXPECT_NEAR(norm_random.z, -0.3310f, 0.01);
}