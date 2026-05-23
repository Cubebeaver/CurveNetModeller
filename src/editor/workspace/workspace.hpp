#pragma once

#include <imgui.h>

enum ModifierKey {
    Ctrl  = 0b0001,
    Shift = 0b0010,
    Alt   = 0b0100,
    Super = 0b1000
};

typedef struct {
    ImGuiKey key;
    ModifierKey modifier;
} KeyPressedEventArgs;

class Workspace {
private:
    //TODO jobb input kezelés, főleg a drgot, hogy a click és a drag jobban elkülönüljön...

protected:

    void HandleInput() {

    }


public:
    virtual void Draw() = 0;

    virtual ~Workspace() = default;
};