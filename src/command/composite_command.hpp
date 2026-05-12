#pragma once
#include <list>
#include <memory>

#include "i_command.hpp"


class CompositeCommand : public ICommand {
private:
    std::list<std::shared_ptr<ICommand>> commands;

public:
    static std::shared_ptr<CompositeCommand> Create() {
        return std::make_shared<CompositeCommand>();
    }

    CompositeCommand& Add(std::shared_ptr<ICommand> command) {
        commands.push_back(command);
        return *this;
    }
    template<typename T, typename... Args>
    CompositeCommand& Add(Args&&... args) {
        commands.push_back(std::make_shared<T>(std::forward<Args>(args)...));
        return *this;
    }

    CompositeCommand& ExecuteAdd(std::shared_ptr<ICommand> command) {
        command->Execute();
        commands.push_back(command);
        return *this;
    }
    template<typename T, typename... Args>
    CompositeCommand& ExecuteAdd(Args&&... args) {
        auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
        cmd->Execute();
        commands.push_back(cmd);
        return *this;
    }

    virtual void Execute() override {
        for (auto& command : commands) {
            command->Execute();
        }
    }

    virtual void Undo() override {
        for (auto it = commands.rbegin(); it != commands.rend(); ++it) {
            (*it)->Undo();
        }
    }
};
