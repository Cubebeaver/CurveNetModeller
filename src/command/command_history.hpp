#pragma once

#include <memory>
#include <iostream>
#include <list>

#include "i_command.hpp"

class CommandHistory {
private:
    std::list<std::shared_ptr<ICommand>> undoList;
    std::list<std::shared_ptr<ICommand>> redoList;

    CommandHistory() = default;

public:
    CommandHistory(const CommandHistory&) = delete;
    CommandHistory& operator=(const CommandHistory&) = delete;

    static void Do(std::shared_ptr<ICommand> command) {
        GetInstance().do_impl(command);
    }
    template<typename T, typename... Args>
    static void Do(Args&&... args) {
        Do(std::make_shared<T>(std::forward<Args>(args)...));
    }

    static void Undo() {
        GetInstance().undo_impl();
    }
    static void Redo() {
        GetInstance().redo_impl();
    }


protected:
    static CommandHistory& GetInstance() {
        static CommandHistory instance;
        return instance;
    }

    void do_impl(std::shared_ptr<ICommand> command) {
        if (command->Execute()) {
            undoList.push_back(command);

            if (!redoList.empty()) redoList.clear();

            if (undoList.size() > 100) undoList.pop_front();
        }
    }

    template<typename T, typename... Args>
    void do_impl(Args&&... args) {
        do_impl(std::make_shared<T>(std::forward<Args>(args)...));
    }

    void undo_impl() {
        if (undoList.empty()) { std::cout << "No more commands to undo" << std::endl; return; }

        auto command = undoList.back();
        undoList.pop_back();
        redoList.push_back(command);
        command->Undo();
    }

    void redo_impl() {
        if (redoList.empty()) { std::cout << "No more commands to redo" << std::endl; return; }

        auto command = redoList.back();
        redoList.pop_back();
        undoList.push_back(command);
        command->Execute();
    }
};
