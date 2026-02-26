#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "gl_engine/transform.hpp"



//! Transzponáltan kell leírni a mátrixokat, hogy az eltolás az alsó sorban van

TEST(TransformTest, Initialization) {
    Transform t;

    EXPECT_EQ(t.LocalPosition, glm::vec3(0, 0, 0));
    EXPECT_EQ(t.LocalRotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(t.LocalScale, glm::vec3(1, 1, 1));
    EXPECT_EQ(t.LocalMatrix, glm::mat4(1, 0, 0, 0,
                                       0, 1, 0, 0,
                                       0, 0, 1, 0,
                                       0, 0, 0, 1));
    EXPECT_EQ(t.Parent, nullptr);
}

TEST(TransformTest, Translate) {
    Transform t;

    t.Translate(glm::vec3(1, 0, 0));

    EXPECT_EQ(t.LocalPosition, glm::vec3(1, 0, 0));
    EXPECT_EQ(t.LocalRotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(t.LocalScale, glm::vec3(1, 1, 1));
    EXPECT_EQ(t.LocalMatrix, glm::mat4(1, 0, 0, 0,
                                       0, 1, 0, 0,
                                       0, 0, 1, 0,
                                       1, 0, 0, 1));
    EXPECT_EQ(t.Parent, nullptr);
}

TEST(TransformTest, Parenting) {
    Transform parent;
    Transform child;
    child.Parent = &parent;

    parent.Translate(glm::vec3(1, 2, 3));
    parent.Scale(0.5f);
    child.Translate(glm::vec3(4, 6, 8));

    

    EXPECT_EQ(parent.LocalPosition, glm::vec3(1, 2, 3));
    EXPECT_EQ(parent.LocalRotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(parent.LocalScale,    glm::vec3(0.5f, 0.5f, 0.5f));
    EXPECT_EQ(parent.LocalMatrix,   glm::mat4(0.5f, 0, 0, 0,
                                              0, 0.5f, 0, 0,
                                              0, 0, 0.5f, 0,
                                              1, 2, 3, 1.0f));
    EXPECT_EQ(parent.Parent, nullptr);

    EXPECT_EQ(child.LocalPosition, glm::vec3(4, 6, 8));
    EXPECT_EQ(child.LocalRotation, glm::vec3(0, 0, 0));
    EXPECT_EQ(child.LocalScale,    glm::vec3(1, 1, 1));
    EXPECT_EQ(child.LocalMatrix,   glm::mat4(1, 0, 0, 0,
                                             0, 1, 0, 0,
                                             0, 0, 1, 0,
                                             4, 6, 8, 1));
    EXPECT_EQ(child.Parent, &parent);



    EXPECT_EQ(parent.GetWorldMatrix(), glm::mat4(0.5f, 0, 0, 0,
                                                 0, 0.5f, 0, 0,
                                                 0, 0, 0.5f, 0,
                                                 1, 2, 3, 1.0f));
    
    EXPECT_EQ(child.GetWorldMatrix(),  glm::mat4(0.5f, 0, 0, 0,
                                                 0, 0.5f, 0, 0,
                                                 0, 0, 0.5f, 0,
                                                 3, 5, 7, 1.0f));
}