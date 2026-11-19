#pragma once

template <typename T>
int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

template <typename T>
float signf(T val) {
    return static_cast<float>(T(0) < val) - (val < T(0));
}