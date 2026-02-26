#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



//TODO - Itt lehetne ilyen dirty-zőset csinálni, hogy ha a world matrixot is eltároljuk,
//todo   és csak akkor számítjuk újra ha változott valami
class Transform
{
private:

public:
    Transform* Parent = nullptr;

    glm::vec3 LocalPosition;
    glm::vec3 LocalRotation;
    glm::vec3 LocalScale;

    glm::mat4 LocalMatrix;



    void UpdateMatrix() {
        LocalMatrix = glm::mat4(1.0f);
        LocalMatrix = glm::translate(LocalMatrix, LocalPosition);
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        LocalMatrix = glm::scale    (LocalMatrix, LocalScale);
    }
    float* GetMatricPtr() { return glm::value_ptr(LocalMatrix); }

    glm::mat4 GetWorldMatrix() {
        if (Parent) {
            return Parent->GetWorldMatrix() * LocalMatrix;
        } else {
            return LocalMatrix;
        }
    }

    Transform() 
        : LocalPosition(glm::vec3(0.0f, 0.0f, 0.0f)), 
          LocalRotation(glm::vec3(0.0f, 0.0f, 0.0f)), 
          LocalScale(glm::vec3(1.0f, 1.0f, 1.0f)), 
          LocalMatrix(glm::mat4(1.0f)) { }
    Transform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_)
        : LocalPosition(pos), LocalRotation(rot), LocalScale(scale_) { 
        UpdateMatrix(); 
    }
    Transform(glm::vec3 pos, glm::vec3 rot, float scale_)
        : Transform(pos, rot, glm::vec3(scale_, scale_, scale_)) { }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
        : Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ)) { }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale_)
        : Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_)) { }
    ~Transform() {
        std::cout << "[?] Transform::~Transform() not implemented" << std::endl;
    }

    void SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_) {
        LocalPosition = pos;
        LocalRotation = rot;
        LocalScale = scale_;

        UpdateMatrix();
    }
    void SetTransform(glm::vec3 pos, glm::vec3 rot, float scale_) { SetTransform(pos, rot, glm::vec3(scale_, scale_, scale_)); }
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ)); }
    void SetTransform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale_) { SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_)); }

    void SetPosition(glm::vec3 pos) {
        LocalPosition = pos;
        UpdateMatrix();
    }
    void SetRotation(glm::vec3 rot) {
        LocalRotation = rot;
        UpdateMatrix();
    }
    void SetScale   (glm::vec3 scale_) {
        LocalScale = scale_;
        UpdateMatrix();
    }
    void SetPosition(float x, float y, float z) { SetPosition(glm::vec3(x, y, z)); }
    void SetRotation(float x, float y, float z) { SetRotation(glm::vec3(x, y, z)); }
    void SetScale   (float x, float y, float z) { SetScale   (glm::vec3(x, y, z)); }
    void SetScale   (float scale_)              { SetScale   (glm::vec3(scale_, scale_, scale_)); }

    void Translate(glm::vec3 offset) {
        LocalPosition += offset;
        UpdateMatrix();
    }
    void Rotate   (glm::vec3 rotate) {
        LocalRotation += rotate;
        UpdateMatrix();
    }
    void Scale    (glm::vec3 scale_) {
        LocalScale *= scale_;
        UpdateMatrix();
    }
    void Translate(float x, float y, float z) { Translate(glm::vec3(x, y, z)); }
    void Rotate   (float x, float y, float z) { Rotate   (glm::vec3(x, y, z)); }
    void Scale    (float x, float y, float z) { Scale    (glm::vec3(x, y, z)); }
    void Scale    (float scale_)              { Scale    (glm::vec3(scale_, scale_, scale_)); }
};