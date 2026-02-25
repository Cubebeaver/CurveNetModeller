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
    Mesh(const Mesh&) = delete;

    Mesh(const std::vector<VertexType>& vertexBuffer, const std::vector<GLuint>& indexBuffer, GLenum drawMode = GL_STATIC_DRAW)
        : VertexBuffer(vertexBuffer), IndexBuffer(indexBuffer), Stride(0) {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexBuffer.size() * sizeof(VertexType), VertexBuffer.data(), drawMode);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer.size() * sizeof(GLuint), IndexBuffer.data(), drawMode);
    }

    ~Mesh() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    Mesh& AddAttribPointer(int number, unsigned int type ,bool normalized) {
        Bind();
        VertexAttribList.push_back(VertexArrtribListElement(number, type, normalized));
        return *this;
    }
    
    Mesh& FinishVertexAttribs() {
        Bind();

        int stride = 0;
        for (const auto& record : VertexAttribList) {
            stride += record.Number * sizeof(VertexType); 
        }
        this->Stride = stride;

        int offset = 0;
        for (int i = 0; i < VertexAttribList.size(); i++) {
            VertexArrtribListElement& record = VertexAttribList[i];

            glVertexAttribPointer(i, record.Number, record.DataType, record.Normalized ? GL_TRUE : GL_FALSE, Stride, (void*)offset);
            glEnableVertexAttribArray(i);

            offset += record.Number * sizeof(VertexType);
        }

        return *this;
    }

    void Bind() const { glBindVertexArray(VAO); }

    void Draw() const {
        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, IndexBuffer.size(), GL_UNSIGNED_INT, 0);
    }
};