#pragma once

#include <glm/glm.hpp>

char* readFileToString(const char* path);
inline float vector_angle(glm::vec3 A, glm::vec3 B) { return acosf( (A.x*B.x + A.y*B.y + A.z*B.z) / (sqrtf(A.x*A.x + A.y*A.y + A.z*A.z) * sqrtf(B.x*B.x + B.y*B.y + B.z*B.z)) ); }
inline float lerp(float a, float b, float t) { return (b - a) * t + a; }
inline glm::vec3 lerp(glm::vec3 a, glm::vec3 b, float t) { return (b - a) * t + a; }