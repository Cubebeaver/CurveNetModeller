#include <gtest/gtest.h>
#include <glm/glm.hpp>

#include "gl_engine/transform.hpp"



//! Transzponáltan kell leírni a mátrixokat, hogy az eltolás az alsó sorban van
TEST(TransformTest, Initialization) {
    Transform t;

    EXPECT_EQ(t.GetPosition(), glm::vec3(0, 0, 0));
    EXPECT_EQ(t.GetRotation(), glm::vec3(0, 0, 0));
    EXPECT_EQ(t.GetScale(), glm::vec3(1, 1, 1));
    EXPECT_EQ(t.GetLocalMatrix(), glm::mat4(1, 0, 0, 0,
                                            0, 1, 0, 0,
                                            0, 0, 1, 0,
                                            0, 0, 0, 1));
    EXPECT_EQ(t.GetParent().lock(), nullptr);
}

TEST(TransformTest, Translate) {
    Transform t;

    t.Translate(glm::vec3(1, 0, 0));

    EXPECT_EQ(t.GetPosition(), glm::vec3(1, 0, 0));
    EXPECT_EQ(t.GetRotation(), glm::vec3(0, 0, 0));
    EXPECT_EQ(t.GetScale(), glm::vec3(1, 1, 1));
    EXPECT_EQ(t.GetLocalMatrix(), glm::mat4(1, 0, 0, 0,
                                       0, 1, 0, 0,
                                       0, 0, 1, 0,
                                       1, 0, 0, 1));
    EXPECT_EQ(t.GetParent().lock(), nullptr);
}

TEST(TransformTest, Parenting) {
    const std::shared_ptr<Transform> parent = std::make_shared<Transform>();
    const std::shared_ptr<Transform> child = std::make_shared<Transform>();
    child->SetParent(parent);

    parent->Translate(glm::vec3(1, 2, 3));
    parent->Scale(0.5f);
    child->Translate(glm::vec3(4, 6, 8));

    

    EXPECT_EQ(parent->GetPosition(),    glm::vec3(1, 2, 3));
    EXPECT_EQ(parent->GetRotation(),    glm::vec3(0, 0, 0));
    EXPECT_EQ(parent->GetScale(),       glm::vec3(0.5f, 0.5f, 0.5f));
    EXPECT_EQ(parent->GetLocalMatrix(), glm::mat4(0.5f, 0, 0, 0,
                                                  0, 0.5f, 0, 0,
                                                  0, 0, 0.5f, 0,
                                                  1, 2, 3, 1.0f));
    EXPECT_EQ(parent->GetParent().lock(), nullptr);

    EXPECT_EQ(child->GetPosition(),    glm::vec3(4, 6, 8));
    EXPECT_EQ(child->GetRotation(),    glm::vec3(0, 0, 0));
    EXPECT_EQ(child->GetScale(),       glm::vec3(1, 1, 1));
    EXPECT_EQ(child->GetLocalMatrix(), glm::mat4(1, 0, 0, 0,
                                                 0, 1, 0, 0,
                                                 0, 0, 1, 0,
                                                 4, 6, 8, 1));
    EXPECT_EQ(child->GetParent().lock().get(), parent.get());



    EXPECT_EQ(parent->GetWorldMatrix(), glm::mat4(0.5f, 0, 0, 0,
                                                  0, 0.5f, 0, 0,
                                                  0, 0, 0.5f, 0,
                                                  1, 2, 3, 1.0f));
    
    EXPECT_EQ(child->GetWorldMatrix(), glm::mat4(0.5f, 0, 0, 0,
                                                 0, 0.5f, 0, 0,
                                                 0, 0, 0.5f, 0,
                                                 3, 5, 7, 1.0f));
}