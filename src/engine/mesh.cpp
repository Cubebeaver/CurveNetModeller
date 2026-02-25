#include "engine/mesh.h"

#include <vector>
#include <fstream>
#include <glad/glad.h>







Mesh::Mesh(unsigned int numVertices, float* vertices_, unsigned int numIndices, int* indices_, int stride_, unsigned int drawMode) {
    countVertices = numVertices;
    countIndices  = numIndices;
    vertices = vertices_;
    indices  = indices_;
    Stride = stride_;

    glGenVertexArrays(1, &VAO);
    // Bind VAO
    glBindVertexArray(VAO);

    // Set up VBO
    glGenBuffers(1, &VBO);
    // Az VBO-nak a vertex_buffer tipusa?, ezutan ahanyszor vmi valtoztatast vegzunk a GL_ARRAY_BUFFER-en, az az itt kivalasztott VBO-ra fog vonatkozni
    // Lehet egyszerre tobb buffert is bind-elni de kulonbozo tipusainak kell lenniuk, es az egyszerre bindel-t buffereken lehet munkat vegezni, at-bind-eles nelkul
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Bemasolja a kivalasztott adatot a jelenleg bind-elt GL_ARRAY_BUFFER-be
    // usuage lehet:
    //   - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    //   - GL_STATIC_DRAW: the data is set only once and used many times.
    //   - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, countVertices * sizeof(float), vertices, drawMode);

    // Set up EBO
    //EBO, element buffer object, ami az vertex index array
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, countIndices * sizeof(int), indices, drawMode);

    // Eddig a VAO ezeket megismetli majd helyettunk igy eleg csak a VAO-t bind-elni

    glBindVertexArray(0);

    //printf("[-] Mesh::Mesh(float* vertices_, int* indices_, unsigned int drawMode) not implemented");
}

Mesh::Mesh(const char* objFilePath, unsigned int drawMode) {
// Binary Data layout:
//
// +-------------------+---------------------+--------------------+
// | hasColor : 1 byte | hasTexture : 1 byte | hasNormal : 1 byte | ...
// +-------------------+---------------------+--------------------+
// 
// +-----------------------------------+------------------------------ -       - -------------+
// | vertData length in bytes : 4 byte | vertData represented by floats  . . .  4 byte blocks | ...
// +-----------------------------------+------------------------------ -       - -------------+
// 
// +----------------------------------+--------------------------- -       - -------------+
// | indData length in bytes : 4 byte | indData represented by ints  . . .  4 byte blocks | end
// +----------------------------------+--------------------------- -       - -------------+
//

    // Read binary data
    std::ifstream bin(objFilePath, std::ios::binary);

    bool hasColor = false, hasTexture = false, hasNormal = false;
    int nVert = -1, nInd = -1;

    // Read properties
    bin.read((char*)&hasColor, 1);
    bin.read((char*)&hasTexture, 1);
    bin.read((char*)&hasNormal, 1);

    // Read vertices
    bin.read((char*)&nVert, 4);             // Number of bytes of vertices
    countVertices = nVert / sizeof(float);  // Number of floats of vertices
    char* bufVert = new char[nVert];        // Create buffer for binary data
    bin.read(bufVert, nVert);               // Read binary data
    vertices = (float*)bufVert;             // Assign buffer to vertices

    bin.read((char*)&nInd, 4);              // Number of bytes of indices
    countIndices = nInd / sizeof(int);      // Number of ints of indices
    char* bufInd = new char[nInd];          // Create buffer for binary data
    bin.read(bufInd, nInd);                 // Read binary data
    indices = (int*)bufInd;                 // Assign buffer to indices


    // Set up VAO
    // VAO, ezt eltarolja hogy az adott mesh kirajzolasahoz milyen bindeleseket (: VBO, vert.attribute) kell vegezni hogy kirajzolhatok legyenek
    // FONTOS VAO-val kapcsolatban hogy minden bindbuffer call-t elment, tehat azt is amikor unbind-elunk valamit ezert oda kell figyelni hogy ha manipulaljuk a dolgokat nehogy valami rosszat jegyezzen meg a VAO
    glGenVertexArrays(1, &VAO);
    // Bind VAO
    glBindVertexArray(VAO);

    // Set up VBO
    glGenBuffers(1, &VBO);
    // Az VBO-nak a vertex_buffer tipusa?, ezutan ahanyszor vmi valtoztatast vegzunk a GL_ARRAY_BUFFER-en, az az itt kivalasztott VBO-ra fog vonatkozni
    // Lehet egyszerre tobb buffert is bind-elni de kulonbozo tipusainak kell lenniuk, es az egyszerre bindel-t buffereken lehet munkat vegezni, at-bind-eles nelkul
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Bemasolja a kivalasztott adatot a jelenleg bind-elt GL_ARRAY_BUFFER-be
    // usuage lehet:
    //   - GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    //   - GL_STATIC_DRAW: the data is set only once and used many times.
    //   - GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, countVertices * sizeof(float), vertices, drawMode);

    // Set up EBO
    //EBO, element buffer object, ami az vertex index array
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, countIndices * sizeof(int), indices, drawMode);

    // Stride
    /* position */  Stride =  3;
    if (hasColor)   Stride += 3;
    if (hasTexture) Stride += 2;
    if (hasNormal)  Stride += 3;

    // Set up the vertex layout
    AddVertexAttribute(GL_FLOAT, 3, false);
    if (hasColor)   AddVertexAttribute(GL_FLOAT, 3, false);
    if (hasTexture) AddVertexAttribute(GL_FLOAT, 2, false);
    if (hasNormal)  AddVertexAttribute(GL_FLOAT, 3, false);
    // Eddig a VAO ezeket megismetli majd helyettunk igy eleg csak a VAO-t bind-elni

    glBindVertexArray(0);

    bin.close();
}

Mesh::~Mesh() {
    if (vertices != nullptr) delete[] vertices;
    if (indices != nullptr)  delete[] indices;

    glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDeleteBuffers(1, &VBO);
    //VBO = 0;
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO);
    glDeleteBuffers(1, &EBO);
    //EBO = 0;
    glDeleteVertexArrays(1, &VAO);
    //VAO = 0;
    glBindVertexArray(0);

#ifdef DEBUG
    printf("[?] Mesh::~Mesh() implemented???\n");
#endif
}

void Mesh::UpdateVertices(unsigned int numVertices, float* vertices_, unsigned int drawMode) {
    glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDeleteBuffers(1, &VBO);
    //VBO = 0;

    countVertices = numVertices;
    if (vertices != nullptr) delete[] vertices;
    vertices = vertices_;

    glCreateBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, countVertices * sizeof(float), vertices, drawMode);
    
    glBindVertexArray(0);
}

void Mesh::UpdateIndices(unsigned int numIndices, int* indices_, unsigned int drawMode) {
    glBindVertexArray(VAO);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDeleteBuffers(1, &EBO);
    //EBO = 0;

    countIndices = numIndices;
    if (indices != nullptr) delete[] indices;
    indices = indices_;

    glCreateBuffers(0, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, countIndices * sizeof(int), indices, drawMode);

    glBindVertexArray(0);
}

void Mesh::AddVertexAttribute(unsigned int dataType, int number, bool normalized = false) {
    // Linking vertex attributes
    //  glVertexAttribPointer(      Megmondhatjuk hogy hogyan kezelje az egyes vertexek adatait
    //      GLuint index,           ez a (position = 0) a vert shaderben
    //      GLint size,             Hany ertekbol all az attribute, vec3 eseteben 3
    //      GLenum type,            Adat tipusa ez esetben GL_FLOAT
    //      GLboolean normalized,   This argument specifies if we want the data to be normalized. If we’re inputting integer data types (int, byte) and we’ve set this to GL_TRUE, the integer data is normalized to 0 (or -1 for signed data) and 1 when converted to float. This is not relevant for us so we’ll leave this at GL_FALSE.
    //      GLsizei stride,         A vertexek kezdete kozotti tavolsag, azaz mekkora a stride (lehet padding? azaz != a vertex meretevel)
    //      const void *pointer     Az adat offsetje a vertexen belul
    //  )

    glBindVertexArray(VAO);

    int offset = 0;
    for (int i = 0; i < vertexArrtribList.size(); i++) {
        offset += vertexArrtribList[i].number * sizeof(float);
    }

    glVertexAttribPointer(vertexArrtribList.size(), number, dataType, normalized ? GL_TRUE : GL_FALSE, Stride * sizeof(float), (void*)offset);
    EnableVertexAttribute(vertexArrtribList.size()); // Hanyadik vertex attribute-ot enable-olja be?

    vertexArrtribList.push_back(VertexArrtribListElement(number, dataType, normalized));

    glBindVertexArray(0);
}