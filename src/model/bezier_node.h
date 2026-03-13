#pragma once

#include <glm/glm.hpp>

#include "util/event.hpp"


enum class HandleType {
    None = 0b0,
    Center = 0b1,
    Left = 0b10,
    Right = 0b100,
};

enum class HandleMode {
    Free,
    Aligned,
    Symmetric
};

class BezierNode {
public:
    glm::vec3 Position;
    glm::vec3 LeftHandle;
    glm::vec3 RightHandle;
    HandleMode Mode;

    Event<> BezierNodeChanged;
    
    BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode = HandleMode::Symmetric);
    BezierNode(HandleMode mode = HandleMode::Aligned);
    BezierNode(glm::vec3 position, HandleMode mode = HandleMode::Aligned);

    const glm::vec3& GetPosition() const { return Position; }
    glm::vec3& GetPosition() { return Position; }
    void SetPosition(const glm::vec3& newPos);
    
    const glm::vec3& GetLeftHandle() const { return LeftHandle; }
    glm::vec3& GetLeftHandle() { return LeftHandle; }
    void SetLeftHandle(const glm::vec3& newPos);
    
    const glm::vec3& GetRightHandle() const { return RightHandle; }
    glm::vec3& GetRightHandle() { return RightHandle; }
    void SetRightHandle(const glm::vec3& newPos);
    
    const HandleMode& GetMode() const { return Mode; }
    HandleMode& GetMode() { return Mode; }
    void SetMode(HandleMode newMode);

private:
    void EnforceMode(bool isLeftChanged);
};