#include "GlfwWindow.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

int main() {

    const GlfwWindow window(1920, 1080, "Hello OpenGL");

    const ShaderProgram shader;
    shader.attach(GL_VERTEX_SHADER, "../shaders/basic.vert");
    shader.attach(GL_FRAGMENT_SHADER, "../shaders/basic.frag");
    shader.link();
    shader.use();

    // 开启 Depth Test 避免 3D 问题 （可能未出现）
    glEnable(GL_DEPTH_TEST);

    // 关闭 VSync
    glfwSwapInterval(0);

    float vertices[] = {// pos        // color
                        -1.0f, -1.0f, 0.0f, 1, 0, 0, // 0
                        1.0f,  -1.0f, 0.0f, 0, 1, 0, // 1
                        1.0f,  1.0f,  0.0f, 0, 0, 1, // 2
                        -1.0f, 1.0f,  0.0f, 1, 1, 0, // 3
                        0.0f,  0.0f,  1.0f, 0, 0, 0}; // 4

    unsigned int indices[] = {
            0, 1, 2, // 1
            2, 3, 0, // 2
            0, 1, 4, // 3
            0, 2, 4, // 4
            2, 1, 4, // 5
            2, 3, 4 // 6

    };

    const Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(unsigned int));

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const int timeLoc = glGetUniformLocation(shader.id, "uTime");

    // 创建 camera
    Camera camera{};

    // 注册鼠标回调函数
    glfwSetWindowUserPointer(window.as_ptr(), &camera); // 将 camera 的指针传递给回调函数
    auto mouse_callback = [](GLFWwindow *win, const double xpos, const double ypos) {
        auto *cam = static_cast<Camera *>(glfwGetWindowUserPointer(win)); // 获取 camera 的指针

        static float lastX = 960, lastY = 540;
        static bool firstMouse = true;

        if (firstMouse) {
            lastX = static_cast<float>(xpos);
            lastY = static_cast<float>(ypos);
            firstMouse = false;
        }

        const auto xoffset = static_cast<float>(xpos - lastX);
        const auto yoffset = static_cast<float>(lastY - ypos);
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);

        cam->ProcessMouse(xoffset, yoffset);
    };
    glfwSetCursorPosCallback(window.as_ptr(), mouse_callback); // 注册

    // 捕获鼠标
    glfwSetInputMode(window.as_ptr(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // 初始化 deltaTime
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window.as_ptr())) {
        // 计算 deltaTime
        const auto current = static_cast<float>(glfwGetTime());
        deltaTime = current - lastFrame;
        lastFrame = current;

        // 处理键盘输入
        if (glfwGetKey(window.as_ptr(), GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(GLFW_KEY_W, deltaTime);
        if (glfwGetKey(window.as_ptr(), GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(GLFW_KEY_S, deltaTime);
        if (glfwGetKey(window.as_ptr(), GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(GLFW_KEY_A, deltaTime);
        if (glfwGetKey(window.as_ptr(), GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(GLFW_KEY_D, deltaTime);

        const auto timeValue = static_cast<float>(glfwGetTime());
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto model = glm::mat4(1.0f);
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1920.0f / 1080.0f, 0.1f, 100.0f);

        glm::mat4 mvp = projection * view * model;

        const int mvpLoc = glGetUniformLocation(shader.id, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        shader.use();
        glUniform1f(timeLoc, timeValue);

        // 调用 draw() 方法直接绘制出 Mesh
        mesh.draw();

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}
