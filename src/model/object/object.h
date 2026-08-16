#pragma once

#include <memory>

#include "transform.h"


class Object {
private:
    std::shared_ptr<Transform> transform;

public:
    Object() : transform(std::make_shared<Transform>()) { }

    virtual void InitializeAfterLoad() = 0;

    std::shared_ptr<Transform> GetTransform() const { return transform; }

    virtual ~Object() = default;
};
