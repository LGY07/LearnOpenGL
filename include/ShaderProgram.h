#pragma once
#include <string>
#include "glad/glad.h"

class ShaderProgram {
    static std::string loadFromFile(const char *path);
    static unsigned int compileShader(unsigned int type, const char *src);

public:
    unsigned int id;
    ShaderProgram();
    ~ShaderProgram();

    void attach(unsigned int type, const char *path) const;
    void link() const;
    void use() const { glUseProgram(id); }
};
