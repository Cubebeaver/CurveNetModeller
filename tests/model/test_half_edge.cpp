#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/element/bezier_curve.h"
#include "model/element/bezier_node.h"
#include "model/element/half_edge.hpp"



// Teszteljük, hogy a konstruktor jól állítja-e be az alapértékeket
TEST(HalfEdgeTest, Initialization) {
    std::shared_ptr<BezierNode> node1 = std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    std::shared_ptr<BezierNode> node2 = std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3( 2.0f, 1.0f, 0.0f));

    std::shared_ptr<BezierCurve> curve = std::make_shared<BezierCurve>();
    curve->AddNode(node1);
    curve->AddNode(node2);

    auto point = curve->EvaluatePosition(0.5f);
    EXPECT_NEAR(point.x, 0.0f, 0.001);
    EXPECT_NEAR(point.y, -0.75f, 0.001f);
    EXPECT_NEAR(point.z, 0.0f, 0.001f);

    auto he_forward = std::make_shared<HalfEdge>(curve);
    auto he_point_f = he_forward->EvaluatePosition(0.5f);
    EXPECT_NEAR(he_point_f.x, 0.0f, 0.001);
    EXPECT_NEAR(he_point_f.y, -0.75f, 0.001f);
    EXPECT_NEAR(he_point_f.z, 0.0f, 0.001f);

    auto he_backward = std::make_shared<HalfEdge>(curve);
    auto he_point_b = he_backward->EvaluatePosition(0.5f);
    EXPECT_NEAR(he_point_b.x, 0.0f, 0.001);
    EXPECT_NEAR(he_point_b.y, -0.75f, 0.001f);
    EXPECT_NEAR(he_point_b.z, 0.0f, 0.001f);
}

