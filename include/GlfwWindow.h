#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

class GlfwWindow {
    GLFWwindow *window = nullptr;

public:
    GlfwWindow(int width, int height, const char *title);
    ~GlfwWindow();

    [[nodiscard]] GLFWwindow *as_ptr() const;
};
