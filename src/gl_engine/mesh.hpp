#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glad/glad.h>

namespace gl_engine {

typedef struct VertexArrtribListElement {
    int Number;
    unsigned int DataType;
    bool Normalized;

    VertexArrtribListElement(int number, unsigned int type, bool normalized)
        : Number(number), DataType(type), Normalized(normalized) { }
} VertexArrtribListElement;



template<typename VertexType, typename VertexElementType>
class MeshBase {
    GLuint VBO, EBO, VAO;
    
    int VertexBufferBytes;
    int IndexBufferBytes;

    int Stride;
    std::vector<VertexArrtribListElement> VertexAttribList;

public:
    MeshBase(const MeshBase&) = delete;

    MeshBase(const std::vector<VertexType>& vertexBuffer, const std::vector<GLuint>& indexBuffer, GLenum drawMode = GL_STATIC_DRAW)
        : Stride(0) {
        VertexBufferBytes = vertexBuffer.size() * sizeof(VertexType);
        IndexBufferBytes = indexBuffer.size() * sizeof(GLuint);
        
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexBufferBytes, vertexBuffer.data(), drawMode);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferBytes, indexBuffer.data(), drawMode);
    }

    ~MeshBase() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void ReplaceVertices(const std::vector<VertexType>& newVertices) {
        Bind();
        VertexBufferBytes = newVertices.size() * sizeof(VertexType);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, VertexBufferBytes, newVertices.data(), GL_STATIC_DRAW);
    }
    void ReplaceIndices(const std::vector<GLuint>& newIndices) {
        Bind();
        IndexBufferBytes = newIndices.size() * sizeof(GLuint);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, IndexBufferBytes, newIndices.data(), GL_STATIC_DRAW);
    }
    void Replace(const std::vector<VertexType>& newVertices, const std::vector<GLuint>& newIndices) {
        ReplaceVertices(newVertices);
        ReplaceIndices(newIndices);
    }

    void UpdateVertices(const std::vector<VertexType>& newVertices, int offset = 0) {
        Bind();
        int newVerticesBytes = newVertices.size() * sizeof(VertexType);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(VertexType), newVerticesBytes, newVertices.data());
    }
    void UpdateIndices(const std::vector<GLuint>& newIndices, int offset = 0) {
        Bind();
        int newIndicesBytes = newIndices.size() * sizeof(GLuint);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset * sizeof(GLuint), newIndicesBytes, newIndices.data());
    }
    void Update(const std::vector<VertexType>& newVertices, const std::vector<GLuint>& newIndices,
                int vertex_offset = 0, int index_offset = 0) {
        UpdateVertices(newVertices, vertex_offset);
        UpdateIndices (newIndices,  index_offset);
    }

    MeshBase& AddAttribPointer(int number, unsigned int type ,bool normalized) {
        Bind();
        VertexAttribList.push_back(VertexArrtribListElement(number, type, normalized));
        return *this;
    }
    
    MeshBase& FinishVertexAttribs() {
        Bind();

        int stride = 0;
        for (const auto& record : VertexAttribList) {
            stride += record.Number * sizeof(VertexElementType); 
        }
        this->Stride = stride;

        int offset = 0;
        for (int i = 0; i < VertexAttribList.size(); i++) {
            const VertexArrtribListElement& record = VertexAttribList[i];

            glVertexAttribPointer(i, record.Number, record.DataType, record.Normalized ? GL_TRUE : GL_FALSE, Stride, reinterpret_cast<void*>(static_cast<unsigned long long>(offset)));
            glEnableVertexAttribArray(i);

            offset += record.Number * sizeof(VertexElementType);
        }

        return *this;
    }

    void Bind() const { glBindVertexArray(VAO); }

    void Draw(GLenum mode = GL_TRIANGLES) const {
        Bind();

        glDrawElements(mode, IndexBufferBytes / sizeof(GLuint), GL_UNSIGNED_INT, 0);
    }

    void DrawPartial(int start, int end, GLenum mode = GL_TRIANGLES) const {
        Bind();

        glDrawElements(mode, end - start, GL_UNSIGNED_INT, reinterpret_cast<void*>(start * sizeof(GLuint)));
    }
};

class Mesh : public MeshBase<float, float> {
public:
    Mesh(const std::vector<float>& vertexBuffer, const std::vector<GLuint>& indexBuffer, GLenum drawMode = GL_STATIC_DRAW)
        : MeshBase<float, float>(vertexBuffer, indexBuffer, drawMode) { }
};

} // namespace gl_engine