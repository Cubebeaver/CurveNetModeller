#pragma once

#include <memory>

#include "transform.hpp"


class Object {
private:
    std::shared_ptr<Transform> transform;

public:
    std::shared_ptr<Transform> GetTransform() const { return transform; }

    virtual ~Object() = default;
};
