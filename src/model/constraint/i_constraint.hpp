#pragma once

class IConstraint {
public:
    virtual void Enforce() = 0;
    virtual bool Verify() const = 0;

    virtual ~IConstraint() = default;
};