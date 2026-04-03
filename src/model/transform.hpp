#pragma once

#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>



//TODO - Itt lehetne ilyen dirty-zőset csinálni, hogy ha a world matrixot is eltároljuk,
//todo   és csak akkor számítjuk újra ha változott valami
class Transform : public std::enable_shared_from_this<Transform> {
private:
    std::weak_ptr<Transform> Parent;
    std::vector<std::weak_ptr<Transform>> Children;

    glm::vec3 LocalPosition;
    glm::vec3 LocalRotation;
    glm::vec3 LocalScale;

    glm::mat4 LocalMatrix;
    bool dirty_local = true;

public:
    void UpdateLocalMatrix() {
        LocalMatrix = glm::mat4(1.0f);
        LocalMatrix = glm::translate(LocalMatrix, LocalPosition);
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        LocalMatrix = glm::scale    (LocalMatrix, LocalScale);
    }
    glm::mat4 GetLocalMatrix() {
        if (dirty_local) UpdateLocalMatrix();
        return LocalMatrix;
    }
    glm::mat4 GetWorldMatrix() {
        if (dirty_local) UpdateLocalMatrix();

        if (auto p = Parent.lock()) {
            return p->GetWorldMatrix() * LocalMatrix;
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
        UpdateLocalMatrix();
    }
    Transform(glm::vec3 pos, glm::vec3 rot, float scale_)
        : Transform(pos, rot, glm::vec3(scale_, scale_, scale_)) { }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scaleX, float scaleY, float scaleZ)
        : Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ)) { }
    Transform(float posX, float posY, float posZ, float rotX, float rotY, float rotZ, float scale_)
        : Transform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_)) { }

    glm::vec3 GetPosition() { return LocalPosition; }
    glm::vec3 GetRotation() { return LocalRotation; }
    glm::vec3 GetScale() { return LocalScale; }
    std::weak_ptr<Transform> GetParent() { return Parent; }
    const std::vector<std::weak_ptr<Transform>>& GetChildren() const { return Children; }

    void SetParent(std::shared_ptr<Transform> parent) {
        Parent = parent;

        parent->Children.push_back(shared_from_this());
    }

    void SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_) {
        LocalPosition = pos;
        LocalRotation = rot;
        LocalScale = scale_;

        dirty_local = true;
    }
    void SetTransform(glm::vec3 pos, glm::vec3 rot, float scale_) {
        SetTransform(pos, rot, glm::vec3(scale_, scale_, scale_));
    }
    void SetTransform(float posX, float posY, float posZ,
                      float rotX, float rotY, float rotZ,
                      float scaleX, float scaleY, float scaleZ) {
        SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scaleX, scaleY, scaleZ));
    }
    void SetTransform(float posX, float posY, float posZ,
                      float rotX, float rotY, float rotZ,
                      float scale_) {
        SetTransform(glm::vec3(posX, posY, posZ), glm::vec3(rotX, rotY, rotZ), glm::vec3(scale_, scale_, scale_));
    }

    void SetPosition(glm::vec3 pos) {
        LocalPosition = pos;
        dirty_local = true;
    }
    void SetRotation(glm::vec3 rot) {
        LocalRotation = rot;
        dirty_local = true;
    }
    void SetScale   (glm::vec3 scale_) {
        LocalScale = scale_;
        dirty_local = true;
    }
    void SetPosition(float x, float y, float z) { SetPosition(glm::vec3(x, y, z)); }
    void SetRotation(float x, float y, float z) { SetRotation(glm::vec3(x, y, z)); }
    void SetScale   (float x, float y, float z) { SetScale   (glm::vec3(x, y, z)); }
    void SetScale   (float scale_)              { SetScale   (glm::vec3(scale_, scale_, scale_)); }

    void Translate(glm::vec3 offset) {
        LocalPosition += offset;
        dirty_local = true;
    }
    void Rotate   (glm::vec3 rotate) {
        LocalRotation += rotate;
        dirty_local = true;
    }
    void Scale    (glm::vec3 scale_) {
        LocalScale *= scale_;
        dirty_local = true;
    }
    void Translate(float x, float y, float z) { Translate(glm::vec3(x, y, z)); }
    void Rotate   (float x, float y, float z) { Rotate   (glm::vec3(x, y, z)); }
    void Scale    (float x, float y, float z) { Scale    (glm::vec3(x, y, z)); }
    void Scale    (float scale_)              { Scale    (glm::vec3(scale_, scale_, scale_)); }
};