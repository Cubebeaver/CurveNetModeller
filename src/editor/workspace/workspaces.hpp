#pragma once
#include <memory>

#include "properties.hpp"
#include "viewport.hpp"

class Workspaces {
public:
    inline static std::weak_ptr<Viewport> viewport;
    inline static std::weak_ptr<Properties> properties;
};
