#pragma once

#include <glm/glm.hpp>

#include "util/event.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <util/glm_serializer.hpp>

#include "point.h"


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

class BezierNode : public IElement {
private:
    std::shared_ptr<Point> CenterHandle;
    std::shared_ptr<Point> LeftHandle;
    std::shared_ptr<Point> RightHandle;
    HandleMode Mode;

public:
    Event<> BezierNodeChanged;
    
    BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode = HandleMode::Symmetric);
    BezierNode(HandleMode mode = HandleMode::Aligned);
    BezierNode(glm::vec3 position, HandleMode mode = HandleMode::Aligned);

    std::shared_ptr<Point> GetCenterHandle() const { return CenterHandle; }
    void SetPosition(const glm::vec3& newPos);
    void SetPosition(const std::shared_ptr<Point> newPoint);
    
    std::shared_ptr<Point> GetLeftHandle() const { return LeftHandle; }
    void SetLeftHandle(const glm::vec3& newPos);
    void SetLeftHandle(const std::shared_ptr<Point> newPoint);

    std::shared_ptr<Point> GetRightHandle() const { return RightHandle; }
    void SetRightHandle(const glm::vec3& newPos);
    void SetRightHandle(const std::shared_ptr<Point> newPoint);

    const HandleMode& GetMode() const { return Mode; }
    HandleMode& GetMode() { return Mode; }
    void SetMode(HandleMode newMode);

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(CenterHandle), CEREAL_NVP(LeftHandle), CEREAL_NVP(RightHandle));
    }

private:
    void EnforceMode(bool isLeftChanged);
};