#pragma once
#include <iostream>
#include <glm/vec3.hpp>

std::ostream& operator << (std::ostream& lhs, const glm::vec3& rhs) {
    lhs << "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
    return lhs;
}
