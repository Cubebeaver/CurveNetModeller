#pragma once

#include <unordered_map>
#include <string>
#include <memory>

#include "shader.hpp"
#include "util/registry.hpp"

namespace gl_engine {

class SharedShaders : public Registry<std::string, std::shared_ptr<Shader>> { };

} // namespace gl_engine