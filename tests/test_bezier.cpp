#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "model/bezier_node.h"



// Teszteljük, hogy a konstruktor jól állítja-e be az alapértékeket
TEST(BezierNodeTest, Initialization) {
    glm::vec3 startPos(5.0f, 10.0f, 0.0f);
    BezierNode node(startPos);

    EXPECT_EQ(node.Position.x, 5.0f);
    EXPECT_EQ(node.Position.y, 10.0f);
    EXPECT_EQ(node.Mode, HandleMode::Aligned);
}

// Teszteljük a szimmetrikus mód kényszerítését
TEST(BezierNodeTest, SymmetricHandleMovement) {
    BezierNode node(glm::vec3(0.0f, 0.0f, 0.0f));
    node.SetMode(HandleMode::Symmetric);
    
    // Alapból Symmetric módban van. Ha elmozdítjuk a bal handle-t (-2, 0, 0)-ba...
    node.SetLeftHandle(glm::vec3(-2.0f, 0.0f, 0.0f));

    // ...akkor a jobb handle-nek automatikusan a tükörképének kell lennie (2, 0, 0)
    EXPECT_FLOAT_EQ(node.RightHandle.x, 2.0f);
    EXPECT_FLOAT_EQ(node.RightHandle.y, 0.0f);
}

// Teszteljük, hogy a fő pont mozgatása viszi-e a handle-öket is
TEST(BezierNodeTest, MoveCenterUpdatesHandles) {
    BezierNode node(glm::vec3(0.0f, 0.0f, 0.0f));
    
    // A bal handle alapból (-1, 0, 0).
    // Ha eltoljuk a közepét (+5, +5, 0)-val...
    node.SetPosition(glm::vec3(5.0f, 5.0f, 0.0f));

    // ...akkor a bal handle is tolódik: (-1+5, 0+5, 0) = (4, 5, 0)
    EXPECT_FLOAT_EQ(node.LeftHandle.x, 4.0f);
    EXPECT_FLOAT_EQ(node.LeftHandle.y, 5.0f);
}