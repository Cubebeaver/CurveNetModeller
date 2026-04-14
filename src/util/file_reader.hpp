#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <iostream>

inline std::string ReadAllText(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        std::cerr << "[-] File does not exist: " << path << std::endl;
        return "";
    }
    std::ifstream file(path, std::ios::binary);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}