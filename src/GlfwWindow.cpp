#include "GlfwWindow.h"
#include <iostream>

GlfwWindow::GlfwWindow(int width, int height, const char *title) {
    if (!glfwInit())
        throw std::runtime_error("GLFW init failed");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
        throw std::runtime_error("Window init failed");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("GLAD init failed");
}

GlfwWindow::~GlfwWindow() { glfwTerminate(); }

GLFWwindow *GlfwWindow::as_ptr() const { return window; }
