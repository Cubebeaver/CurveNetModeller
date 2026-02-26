#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <iostream>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Feltételezem, hogy ezek a fájlok léteznek a CMakeLists.txt-d alapján
#include "shader.hpp"
#include "texture.hpp"

// Egy típus, ami a felsoroltak közül BÁRMELYIK lehet
using UniformValue = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;

class Material {
private:
    Shader* ShaderProgram;
    std::unordered_map<std::string, UniformValue> Uniforms;
    std::vector<std::pair<std::string, Texture*>> Textures;

public:
    // A Material egy létező Shaderre hivatkozik (nem ő hozza létre!)
    Material(Shader* shader) : ShaderProgram(shader) { }

    // --- Uniform értékek beállítása (CPU memóriában tároljuk) ---
    void SetInt(const std::string& name, int value) { Uniforms[name] = value; }
    void SetFloat(const std::string& name, float value) { Uniforms[name] = value; }
    void SetVec2(const std::string& name, const glm::vec2& value) { Uniforms[name] = value; }
    void SetVec3(const std::string& name, const glm::vec3& value) { Uniforms[name] = value; }
    void SetVec4(const std::string& name, const glm::vec4& value) { Uniforms[name] = value; }
    void SetMat4(const std::string& name, const glm::mat4& value) { Uniforms[name] = value; }

    // --- Textúrák hozzáadása ---
    // A samplerName a shaderben lévő uniform neve, pl.: "diffuseMap"
    void AddTexture(const std::string& samplerName, Texture* texture) {
        Textures.push_back({samplerName, texture});
    }

    // --- A varázslat: A Material aktiválása renderelés előtt ---
    void Bind() const {
        if (!ShaderProgram) {
            std::cerr << "[-] No shader attached" << std::endl;
            return;
        }

        // Unbind old textures
        for (int i = 0; i < 16; i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // 1. Aktiváljuk a shadert
        // (Feltételezem, hogy a Shader osztályodnak van egy Use() vagy Bind() függvénye)
        ShaderProgram->Use(); 

        // 2. Töltsük fel a letárolt uniformokat a GPU-ra
        for (const auto& [name, value] : Uniforms) {
            // Megkeressük a uniform helyét a shaderben (feltételezve, hogy a Shader-nek van publikus ID-ja)
            GLint location = glGetUniformLocation(ShaderProgram->program , name.c_str());
            
            if (location == -1) continue; // Ha nincs ilyen uniform a shaderben (pl. optimalizálta a fordító), ugrunk

            // std::visit: Futásidőben megnézi, mi van a variantban, és aszerint hívja meg a megfelelő OpenGL függvényt
            std::visit([location](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    glUniform1i(location, arg);
                } else if constexpr (std::is_same_v<T, float>) {
                    glUniform1f(location, arg);
                } else if constexpr (std::is_same_v<T, glm::vec2>) {
                    glUniform2fv(location, 1, glm::value_ptr(arg));
                } else if constexpr (std::is_same_v<T, glm::vec3>) {
                    glUniform3fv(location, 1, glm::value_ptr(arg));
                } else if constexpr (std::is_same_v<T, glm::vec4>) {
                    glUniform4fv(location, 1, glm::value_ptr(arg));
                } else if constexpr (std::is_same_v<T, glm::mat4>) {
                    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(arg));
                }
            }, value);
        }

        // 3. Textúrák bekötése a megfelelő textúra slotokba (Texture Units)
        for (int i = 0; i < Textures.size(); ++i) {
            const auto& [samplerName, texture] = Textures[i];
            
            // Textúra bindolása az i. texture slot-ba (Feltételezem, hogy a Texture osztálynak van egy Bind() fv-e)
            texture->BindToUnit(i);
            
            // Beállítjuk a shaderben a samplert, hogy tudja, melyik slotból kell olvasnia (0, 1, 2...)
            GLint location = glGetUniformLocation(ShaderProgram->program, samplerName.c_str());
            if (location != -1) {
                glUniform1i(location, i);
            }
        }
    }
};