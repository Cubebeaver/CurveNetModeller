#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "gl_engine/shader.hpp"

template<typename TKey, typename TValue>
class Registry {
private:
    static inline std::unordered_map<TKey, TValue> products;

public:
    static void Register(TKey id, TValue value) {
        products[id] = value;
    }

    static TValue Get(TKey id) {
        return products[id];
    }
};
