#pragma once

class ICommand {
public:
    /**
     * Execute the operation
     * @return true if successful, false otherwise (in theory it should be always true)
     */
    virtual void Execute() = 0;

    /**
     * Undo the operation
     * @return true if successful, false otherwise (in theory it should be always true)
     */
    virtual void Undo() = 0;

    virtual ~ICommand() = default;
};