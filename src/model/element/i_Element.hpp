#pragma once

class IElement {
public:
    virtual void InitializeAfterLoad() = 0;

    virtual ~IElement() = default;
};