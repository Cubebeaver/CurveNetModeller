#include "transform.h"

void Transform::UpdateLocalMatrix() {
    LocalMatrix = glm::mat4(1.0f);
    LocalMatrix = glm::translate(LocalMatrix, LocalPosition);
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
    LocalMatrix = glm::rotate   (LocalMatrix, LocalRotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    LocalMatrix = glm::scale    (LocalMatrix, LocalScale);
}
glm::mat4 Transform::GetLocalMatrix() {
    if (dirty_local) {
        UpdateLocalMatrix();
        dirty_local = false;
    }
    return LocalMatrix;
}
glm::mat4 Transform::GetWorldMatrix() {
    if (dirty_local) {
        UpdateLocalMatrix();
        dirty_local = false;
    }

    if (auto p = Parent.lock()) {
        return p->GetWorldMatrix() * LocalMatrix;
    } else {
        return LocalMatrix;
    }
}

void Transform::SetParent(std::shared_ptr<Transform> parent) {
    Parent = parent;

    parent->Children.push_back(shared_from_this());
}

void Transform::SetTransform(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale_) {
    LocalPosition = pos;
    LocalRotation = rot;
    LocalScale = scale_;

    dirty_local = true;
}

void Transform::SetPosition(glm::vec3 pos) {
    LocalPosition = pos;
    dirty_local = true;
}
void Transform::SetRotation(glm::vec3 rot) {
    LocalRotation = rot;
    dirty_local = true;
}
void Transform::SetScale(glm::vec3 scale_) {
    LocalScale = scale_;
    dirty_local = true;
}

void Transform::Translate(glm::vec3 offset) {
    LocalPosition += offset;
    dirty_local = true;
}
void Transform::Rotate(glm::vec3 rotate) {
    LocalRotation += rotate;
    dirty_local = true;
}
void Transform::Scale(glm::vec3 scale_) {
    LocalScale *= scale_;
    dirty_local = true;
}