#pragma once

#include <vector>
#include <functional>
#include <algorithm>

template<typename... Args>
class Event {
    std::vector<std::function<void(Args...)>> handlers;

public:
    void operator+=(std::function<void(Args...)> handler) {
        handlers.push_back(handler);
    }

    void Invoke(Args... args) const {
        for (const auto& handler : handlers) {
            handler(args...);
        }
    }
};