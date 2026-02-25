#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>



typedef struct VertexArrtribListElement {
    int Number;
    unsigned int DataType;
    bool Normalized;

    VertexArrtribListElement(int number, unsigned int type, bool normalized)
        : Number(number), DataType(type), Normalized(normalized) { }
} VertexArrtribListElement;



template<typename VertexType = float>
class Mesh {
    GLuint VBO, EBO, VAO;

    std::vector<VertexType> VertexBuffer;
    std::vector<GLuint> IndexBuffer;
    
    int Stride;
    std::vector<VertexArrtribListElement> VertexAttribList;

public:
    Mesh(const std::vector<VertexType>& vertexBuffer, const std::vector<GLuint>& indexBuffer, int stride, GLenum drawMode = GL_STATIC_DRAW) {
        this->VertexBuffer = vertexBuffer;
        this->IndexBuffer = indexBuffer;
        this->Stride = stride;

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexBuffer.size() * sizeof(VertexType), VertexBuffer.data(), drawMode);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer.size() * sizeof(GLuint), IndexBuffer.data(), drawMode);
    }

    void AddAttribPointer(const VertexArrtribListElement& record) {
        glBindVertexArray(VAO);

        int offset = 0;
        for (int i = 0; i < VertexAttribList.size(); i++) {
            offset += VertexAttribList[i].Number * sizeof(VertexType);
        }

        glVertexAttribPointer(VertexAttribList.size(), record.Number, record.DataType, record.Normalized ? GL_TRUE : GL_FALSE, Stride * sizeof(float), (void*)offset);
        glEnableVertexAttribArray(VertexAttribList.size());

        VertexAttribList.push_back(record);
    }

    void Bind() { glBindVertexArray(VAO); }

    void Draw() {
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, IndexBuffer.size(), GL_UNSIGNED_INT, 0);
    }
};