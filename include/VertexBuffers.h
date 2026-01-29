#pragma once
#include <glad/glad.h>

class VertexBuffers {
public:
    unsigned int id{};
    VertexBuffers(float data[], GLsizeiptr size);
    ~VertexBuffers();

    void bind() const;
};
