#pragma once
#include <list>
#include <memory>

#include "i_command.hpp"


class CompositeCommand : public ICommand, public std::enable_shared_from_this<CompositeCommand> {
private:
    std::list<std::shared_ptr<ICommand>> commands;

public:
    static std::shared_ptr<CompositeCommand> Create() {
        return std::make_shared<CompositeCommand>();
    }

    std::shared_ptr<CompositeCommand> Add(std::shared_ptr<ICommand> command) {
        commands.push_back(command);
        return shared_from_this();
    }
    template<typename T, typename... Args>
    std::shared_ptr<CompositeCommand> Add(Args&&... args) {
        commands.push_back(std::make_shared<T>(std::forward<Args>(args)...));
        return shared_from_this();
    }

    std::shared_ptr<CompositeCommand> ExecuteAdd(std::shared_ptr<ICommand> command) {
        command->Execute();
        commands.push_back(command);
        return shared_from_this();
    }
    template<typename T, typename... Args>
    std::shared_ptr<CompositeCommand> ExecuteAdd(Args&&... args) {
        auto cmd = std::make_shared<T>(std::forward<Args>(args)...);
        cmd->Execute();
        commands.push_back(cmd);
        return shared_from_this();
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
