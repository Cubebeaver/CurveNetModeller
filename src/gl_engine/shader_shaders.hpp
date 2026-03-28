#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "shader.hpp"

class ShaderShaders {
private:
    static inline std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

public:
    static void Register(std::string name, std::shared_ptr<Shader> shader) {
        shaders[name] = shader;
    }

    static std::shared_ptr<Shader> Get(std::string name) {
        return shaders[name];
    }
};