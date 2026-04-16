#pragma once
#include "i_Element.hpp"
#include "point.h"

class INode : IElement {
public:
    //TODO
    virtual const std::vector<std::shared_ptr<Point>>& GetPoints() const = 0;

    virtual ~INode() override = default;
};
