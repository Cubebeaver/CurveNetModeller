#pragma once

#include <memory>
#include <vector>
#include <glm/vec3.hpp>

#include "util/event.hpp"
#include "util/observer_generator.hpp"

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

#include "i_Element.hpp"
//#include <iosfwd>

class IConstraint;


//OBSERVER_DEF(Point);

class Point : public IElement {
private:
    glm::vec3 position;

    std::vector<std::shared_ptr<IConstraint>> constraints;

    //OBSERVER_LIST(Point);
public:
    Event<glm::vec3 /* delta */> PointChanged;

    Point(glm::vec3 position) : position(position) { }
    Point() : Point(glm::vec3(0, 0, 0)) { }
    Point(float x, float y, float z) : Point(glm::vec3(x, y, z)) { }

    const glm::vec3& GetPosition() const { return position; }
    glm::vec3& GetPosition() { return position; }
    void SetPosition(const glm::vec3& newPos) {
        if (newPos == position) return; //DEBUG??

        const glm::vec3 delta = newPos - position;
        position = newPos;
        //Notify();
        PointChanged.Invoke(delta);
    }

    const std::vector<std::shared_ptr<IConstraint>>& GetConstraints() const { return constraints; }

    void AddConstraint(std::shared_ptr<IConstraint> constraint) {
        constraints.push_back(constraint);
    }

    void RemoveConstraint(std::weak_ptr<IConstraint> constraint) {
        auto c = constraint.lock();
        if (!c) return;

        constraints.erase(std::remove(constraints.begin(), constraints.end(), c), constraints.end());
        //PointChanged.Invoke(); //A constraint-ben a setposition miatt ez meghívódik majd
    }

    bool IsConstrained() {
        return !constraints.empty();
    }

    virtual ~Point() override = default;

    //OBSERVER_FUNC_HPP

    template<class Archive>
    void serialize(Archive& archive) {
        archive(CEREAL_NVP(position));
    }
};
