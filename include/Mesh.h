#pragma once

#include "glad/glad.h"

class Mesh {
public:
    Mesh(const float *vertices, GLsizei vertexSize, const unsigned int *indices, GLsizei indexCount);

    void draw() const;

    ~Mesh();

private:
    unsigned int vao{};
    unsigned int vbo{};
    unsigned int ebo{};
    GLsizei indexCount{};
};
