# OpenGL 学习笔记（3D 旋转）

## MVP 是啥?

```
MVP = Projection * View * Model
```

* Model：物体自己怎么动（旋转/缩放/平移）

* View：摄像机在哪、朝哪看

* Projection：3D → 2D（透视）

## 修改顶点着色器

旋转过的三角形顶点在屏幕上的投影位置会发生改变，所以需要修改顶点着色器

```
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColOff;

out vec3 vColOff;

uniform mat4 uMVP;

void main()
{
    gl_Position = uMVP * vec4(aPos, 1.0); // 投影后的顶点位置
    vColOff = aColOff;
}
```

## 修改 C++ 程序

```
#include "GlfwWindow.h"
#include "ShaderProgram.h"
#include "VertexArrays.h"
#include "VertexBuffers.h"

【修改 1】

int main() {

    const GlfwWindow window(1920, 1080, "Hello OpenGL");


    const ShaderProgram shader;
    shader.attach(GL_VERTEX_SHADER, "../shaders/basic.vert");
    shader.attach(GL_FRAGMENT_SHADER, "../shaders/basic.frag");
    shader.link();
    shader.use();
    
    【修改 2】

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
        glClear(GL_COLOR_BUFFER_BIT); 【修改 3】
        
        【修改 4】

        shader.use();
        glUniform1f(timeLoc, timeValue);

        VAO.bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}
```

### 【修改 1】

添加 GLM 库，通过 GLM 库完成 MVP 矩阵变换

```
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
```

### 【修改 2】

```
// 开启 Depth Test 避免 3D 问题
glEnable(GL_DEPTH_TEST);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
```

### 【修改 3】

```diff
- glClear(GL_COLOR_BUFFER_BIT);
+ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 同时清理深度缓存
```

### 【修改 4】

```
// Model 三角形自身的旋转
auto model = glm::mat4(1.0f);
model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

// View 相机如何观察
glm::mat4 view = glm::lookAt(
    glm::vec3(0.0f, 0.0f, 3.0f), // 相机位置
    glm::vec3(0.0f, 0.0f, 0.0f), // 看向原点
    glm::vec3(0.0f, 1.0f, 0.0f) // 上方向
    );
    
// Projection 3D -> 2D 透视
glm::mat4 projection = glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f);

/ MVP 合成
glm::mat4 mvp = projection * view * model;

// 通过统一变量传递给顶点着色器
const int mvpLoc = glGetUniformLocation(shader.id, "uMVP");
glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));
```

### 运行即可看到旋转的 3D 三角形