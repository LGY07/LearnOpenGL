#include "ShaderProgram.h"
#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <stdexcept>

#include "glm/gtc/type_ptr.hpp"

std::string ShaderProgram::loadFromFile(const char *path) {
    std::ifstream file(path);
    if (!file)
        throw std::runtime_error(std::string("Failed to open shader file: ") + path);
    auto src = std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
    return src;
}

unsigned int ShaderProgram::compileShader(const unsigned int type, const char *src) {
    const unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char info[512];
        glGetShaderInfoLog(shader, 512, nullptr, info);
        std::cout << info << std::endl;
        throw std::runtime_error(info);
    }
    return shader;
}

ShaderProgram::ShaderProgram() { id = glCreateProgram(); }

ShaderProgram::~ShaderProgram() { glDeleteProgram(id); }

void ShaderProgram::attach(unsigned int type, const char *path) const {
    auto src = loadFromFile(path);
    unsigned int shader = compileShader(type, src.c_str());
    glAttachShader(id, shader);
    glDeleteShader(shader); // attach 后可以删除
}

void ShaderProgram::link() const {
    glLinkProgram(id);
    int success;
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        char info[512];
        glGetProgramInfoLog(id, 512, nullptr, info);
        throw std::runtime_error(info);
    }
}

void ShaderProgram::setFloat(const char *name, const float &value) const {
    const int location = glGetUniformLocation(id, name);
    glUniform1f(location, value);
}

void ShaderProgram::setVec3(const char *name, const glm::vec3 &vec) const {
    const int location = glGetUniformLocation(id, name);
    glUniform3fv(location, 1, glm::value_ptr(vec));
}

void ShaderProgram::setMat4(const char *name, const glm::mat4 &mat) const {
    const int location = glGetUniformLocation(id, name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
}
