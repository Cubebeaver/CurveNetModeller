#pragma once
#include <iostream>
#include <glm/vec3.hpp>

std::ostream& operator << (std::ostream& lhs, const glm::vec3& rhs) {
    lhs << "vec3(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    return lhs;
}

std::ostream& operator << (std::ostream& lhs, const glm::vec2& rhs) {
    lhs << "vec2(" << rhs.x << ", " << rhs.y << ")";
    return lhs;
}
