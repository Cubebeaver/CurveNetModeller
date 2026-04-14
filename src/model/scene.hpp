#pragma once

#include "coons_surface.h"
#include "object.h"

class Scene {
public:
    std::vector<std::shared_ptr<Object>> objects;

    Scene() { }

    void Add(const std::shared_ptr<Object>& obj) {
        objects.push_back(obj);
    }
};
