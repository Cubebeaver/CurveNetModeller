#pragma once
#include <memory>

#include "viewport.hpp"
#include "properties.hpp"

class Workspaces {
public:
    inline static std::weak_ptr<Viewport> viewport;
    inline static std::weak_ptr<Properties> properties;
};
