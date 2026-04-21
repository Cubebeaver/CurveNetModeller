#pragma once

#include <glm/glm.hpp>

#include "util/event.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <util/glm_serializer.hpp>

#include "i_Node.hpp"
#include "point.h"


enum class BezierHandleType {
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

#define CenterHandle points[0]
#define LeftHandle points[1]
#define RightHandle points[2]

class BezierNode : public INode {
private:
    std::vector<std::shared_ptr<Point>> points;
    HandleMode Mode;

public:
    Event<> BezierNodeChanged;
    
    BezierNode(const glm::vec3& position, const glm::vec3& leftHandle, const glm::vec3& rightHandle, HandleMode mode = HandleMode::Symmetric);
    BezierNode(std::shared_ptr<Point> position, std::shared_ptr<Point> leftHandle, std::shared_ptr<Point> rightHandle, HandleMode mode = HandleMode::Symmetric);
    BezierNode(HandleMode mode = HandleMode::Aligned);
    BezierNode(glm::vec3 position, HandleMode mode = HandleMode::Aligned);

    BezierHandleType GetHandleType(std::weak_ptr<Point> point) const;

    virtual const std::vector<std::shared_ptr<Point>>& GetPoints() const override { return points; }

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

    //TODO Az eventekről leiratkozni
    //TODO AZ eventek ne lambdák legyenek, hanem valami konkrétabbak
    virtual ~BezierNode() override;

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(points));
    }

private:
    void EnforceMode(bool isLeftChanged);

    void OnCenterPointChanged(const glm::vec3 offset);
    void OnLeftPointChanged(const glm::vec3 offset);
    void OnRightPointChanged(const glm::vec3 offset);
};

#undef CenterHandle
#undef LeftHandle
#undef RightHandle