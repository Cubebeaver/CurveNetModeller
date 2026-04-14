#pragma once
#include "IInterface.hpp"

class BezierNodeInterface : public IInterface {
public:
    virtual ~BezierNodeInterface() override = default;
    void Draw() override {

    }
};
