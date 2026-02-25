#pragma once

#include <vector>
#include <glad/glad.h>

typedef struct VertexArrtribListElement {
    int number;
    unsigned int type;
    bool normalized;

    VertexArrtribListElement(int _number, unsigned int _type, bool _normalized) : number(_number), type(_type), normalized(_normalized) {}
} VertexArrtribListElement;



class Mesh
{
private:

public:
    float* vertices;
    int countVertices;
    
    int* indices;
    int countIndices;

    int Stride;
    std::vector<VertexArrtribListElement> vertexArrtribList;

    unsigned int VBO, EBO, VAO;

    Mesh() = delete;
    Mesh(unsigned int numVertices, float* vertices_, unsigned int numIndices, int* indices_, int stride_, unsigned int drawMode = GL_STATIC_DRAW);
    Mesh(const char* objFilePath, unsigned int drawMode = GL_STATIC_DRAW);
    ~Mesh();
    void UpdateVertices(unsigned int numVertices, float* vertices_, unsigned int drawMode = GL_STATIC_DRAW);
    void UpdateIndices (unsigned int numIndices,  int* indices_,    unsigned int drawMode = GL_STATIC_DRAW);
    void AddVertexAttribute(unsigned int dataType, int number, bool normalized);
    void EnableVertexAttribute(unsigned int index) { glEnableVertexAttribArray(index); }
    void DisableVertexAttribute(unsigned int index) { glDisableVertexAttribArray(index); }
    void Bind() { glBindVertexArray(VAO); }
};