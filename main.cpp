#include "GlfwWindow.h"
#include "ShaderProgram.h"
#include "VertexArrays.h"
#include "VertexBuffers.h"

int main() {

    const GlfwWindow window(1920, 1080, "Hello OpenGL");


    const ShaderProgram shader;
    shader.attach(GL_VERTEX_SHADER, "../shaders/basic.vert");
    shader.attach(GL_FRAGMENT_SHADER, "../shaders/basic.frag");
    shader.link();
    shader.use();


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
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glUniform1f(timeLoc, timeValue);

        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}
