#pragma once
#include <imgui.h>
#include <memory>

#include "element_interfaces.hpp"
#include "i_interface.hpp"
#include "model/object/object.h"

class ObjectInterface : public IInterface {
private:
    std::weak_ptr<Object> modelObject;

public:
    ObjectInterface(std::shared_ptr<Object> object) : modelObject(object) { }

    virtual void Draw() override {
        if (auto p = modelObject.lock()) {
            ElementInterface::DrawTransformInterface(p->GetTransform());
        }
    }

    virtual ~ObjectInterface() override = default;
};
