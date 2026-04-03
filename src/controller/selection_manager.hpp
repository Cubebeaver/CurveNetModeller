#pragma once
#include <memory>

#include "model/IElement.hpp"

class SelectionManager {
public:
    inline static std::weak_ptr<IElement> selectedElement;
    //inline static std::weak_ptr<IObject> selectedObject;
};
