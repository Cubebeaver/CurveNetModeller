#pragma once

class ICommand {
public:
    /**
     * Execute the operation
     * @return true if successful, false otherwise (in theory it should be always true)
     */
    virtual bool Execute() = 0;

    /**
     * Undo the operation
     * @return true if successful, false otherwise (in theory it should be always true)
     */
    virtual bool Undo() = 0;

    virtual ~ICommand() = default;
};