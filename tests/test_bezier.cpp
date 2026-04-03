#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/bezier_curve.h"
#include "model/bezier_node.h"



// Teszteljük, hogy a konstruktor jól állítja-e be az alapértékeket
TEST(BezierNodeTest, Initialization) {
    glm::vec3 startPos(5.0f, 10.0f, 0.0f);
    BezierNode node(startPos);

    EXPECT_NEAR(node.CenterHandle.x, 5.0f, 0.001f);
    EXPECT_NEAR(node.CenterHandle.y, 10.0f, 0.001f);
    EXPECT_EQ(node.Mode, HandleMode::Aligned);
}

// Teszteljük a szimmetrikus mód kényszerítését
TEST(BezierNodeTest, SymmetricHandleMovement) {
    BezierNode node(glm::vec3(0.0f, 0.0f, 0.0f));
    node.SetMode(HandleMode::Symmetric);
    
    // Alapból Symmetric módban van. Ha elmozdítjuk a bal handle-t (-2, 0, 0)-ba...
    node.SetLeftHandle(glm::vec3(-2.0f, 0.0f, 0.0f));

    // ...akkor a jobb handle-nek automatikusan a tükörképének kell lennie (2, 0, 0)
    EXPECT_NEAR(node.RightHandle.x, 2.0f, 0.001f);
    EXPECT_NEAR(node.RightHandle.y, 0.0f, 0.001f);
}

// Teszteljük, hogy a fő pont mozgatása viszi-e a handle-öket is
TEST(BezierNodeTest, MoveCenterUpdatesHandles) {
    BezierNode node(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // A bal handle alapból (-1, 0, 0).
    // Ha eltoljuk a közepét (+5, +5, 0)-val...
    node.SetPosition(glm::vec3(5.0f, 5.0f, 0.0f));

    // ...akkor a bal handle is tolódik: (-1+5, 0+5, 0) = (4, 5, 0)
    EXPECT_NEAR(node.LeftHandle.x, 4.0, 0.001f);
    EXPECT_NEAR(node.LeftHandle.y, 5.0, 0.001f);
}

TEST(BezierCurveTest, EvaluatePoint) {
    std::shared_ptr<BezierNode> node1 = std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    std::shared_ptr<BezierNode> node2 = std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3( 2.0f, 1.0f, 0.0f));

    BezierCurve curve;
    curve.AddNode(node1);
    curve.AddNode(node2);

    auto point = curve.EvaluateCurve(0.5f);
    EXPECT_NEAR(point.x, 0.0f, 0.001);
    EXPECT_NEAR(point.y, -0.75f, 0.001f);
    EXPECT_NEAR(point.z, 0.0f, 0.001f);
}

TEST(BezierCurveTest, EvaluateNormal) {
    std::shared_ptr<BezierNode> node1 = std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    std::shared_ptr<BezierNode> node2 = std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3( 2.0f, 1.0f, 0.0f));

    BezierCurve curve;
    curve.AddNode(node1);
    curve.AddNode(node2);

    auto normal = curve.EvaluateCurvePrincipalNormal(0.5f);
    EXPECT_NEAR(normal.x, 0.0f, 0.001);
    EXPECT_NEAR(normal.y, 1.0f, 0.001f);
    EXPECT_NEAR(normal.z, 0.0f, 0.001f);
}

TEST(BezierCurveTest, EvaluateCurvature) {
    std::shared_ptr<BezierNode> node1 = std::make_shared<BezierNode>(glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-2.0f, 1.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f));
    std::shared_ptr<BezierNode> node2 = std::make_shared<BezierNode>(glm::vec3( 1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3( 2.0f, 1.0f, 0.0f));

    BezierCurve curve;
    curve.AddNode(node1);
    curve.AddNode(node2);

    auto curvature = curve.EvaluateCurveCurvature(0.5f);
    EXPECT_NEAR(curvature, 2.6666f, 0.001);
}