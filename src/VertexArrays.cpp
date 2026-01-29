#include "VertexArrays.h"

VertexArrays::VertexArrays() { glGenVertexArrays(1, &id); }

VertexArrays::~VertexArrays() { glDeleteVertexArrays(1, &id); }

void VertexArrays::bind() const { glBindVertexArray(id); }
