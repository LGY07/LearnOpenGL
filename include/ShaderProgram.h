#pragma once
#include <string>
#include "glad/glad.h"
#include "glm/fwd.hpp"

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
    void setFloat(const char *name, const float &value) const;
    void setVec3(const char *name, const glm::vec3 &vec) const;
    void setMat4(const char *name, const glm::mat4 &mat) const;
};
