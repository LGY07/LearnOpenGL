#pragma once
#include <glad/glad.h>

class VertexArrays {
public:
    unsigned int id{};
    VertexArrays();
    ~VertexArrays();

    void bind() const;
};
