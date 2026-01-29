#include "VertexBuffers.h"

VertexBuffers::VertexBuffers(float data[], const GLsizeiptr size) {
    glGenBuffers(1, &id);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffers::~VertexBuffers() { glDeleteBuffers(1, &id); }

void VertexBuffers::bind() const { glBindBuffer(GL_ARRAY_BUFFER, id); }
