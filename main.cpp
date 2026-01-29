#include "GlfwWindow.h"
#include "ShaderProgram.h"
#include "VertexArrays.h"
#include "VertexBuffers.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


int main() {

    const GlfwWindow window(1920, 1080, "Hello OpenGL");


    const ShaderProgram shader;
    shader.attach(GL_VERTEX_SHADER, "../shaders/basic.vert");
    shader.attach(GL_FRAGMENT_SHADER, "../shaders/basic.frag");
    shader.link();
    shader.use();

    // 开启 Depth Test 避免 3D 问题 （可能未出现）
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 关闭 VSync
    glfwSwapInterval(0);


    float vertices[] = {
            // pos              // color
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶点 1
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 顶点 2
            0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f // 顶点 3
    };


    const VertexArrays VAO;
    VAO.bind();
    VertexBuffers VBO(vertices, sizeof(vertices));

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    int timeLoc = glGetUniformLocation(shader.id, "uTime");

    while (!glfwWindowShouldClose(window.as_ptr())) {
        const auto timeValue = static_cast<float>(glfwGetTime());
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto model = glm::mat4(1.0f);
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), // 相机位置
                                     glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
                                     glm::vec3(0.0f, 1.0f, 0.0f) // 上方向
        );

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

        glm::mat4 mvp = projection * view * model;

        const int mvpLoc = glGetUniformLocation(shader.id, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));


        shader.use();
        glUniform1f(timeLoc, timeValue);

        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}
