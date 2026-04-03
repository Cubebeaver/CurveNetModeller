#pragma once

class IInterface {
public:
    virtual ~IInterface() = default;

    virtual void Draw() = 0;
};