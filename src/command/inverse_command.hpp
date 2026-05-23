#pragma once
#include <utility>

#include "i_command.hpp"

template <typename T>
class InverseCommand : public ICommand {
    T command;

public:
    template <typename... Args>
    InverseCommand(Args&&... args) : command(std::forward<Args>(args)...) {}

    virtual void Execute() override {
        command.undo();
    }

    virtual void Undo() override {
        command.execute();
    }
};
