# OpenGL 学习笔记（正方形与更多顶点）

## 如何画正方形

显卡只能处理点、线、三角形，一个正方形 = 2 个三角形 = 6 个顶点

但实际上正方形只有 4 个顶点

`A B C`   `A C D`   ← A / C 被重复了

这时候需要用到 EBO（Element Buffer Object），也叫索引缓存

绘制一个正方形需要的顶点数据

```
float vertices[] = {
    // pos        // color
    -0.5f, -0.5f, 0.0f,  1, 0, 0, // 0
     0.5f, -0.5f, 0.0f,  0, 1, 0, // 1
     0.5f,  0.5f, 0.0f,  0, 0, 1, // 2
    -0.5f,  0.5f, 0.0f,  1, 1, 0  // 3
};
```

索引数据

```
unsigned int indices[] = {
    0, 1, 2,   // 第一个三角形
    2, 3, 0    // 第二个三角形
};
```

## 如何使用 EBO

创建 EBO：

操作与 VBO 几乎一样，但一定要注意绑定目标是 `GL_ELEMENT_ARRAY_BUFFER`

```
unsigned int EBO;

glGenBuffers(1, &EBO); // 生成

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO); // 绑定
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); // 上传数据
```

EBO 是 VAO 的一部分，绑定时机很重要

操作顺序：

`绑定 VAO -> 绑定 VBO -> 绑定 EBO -> glVertexAttribPointer()`

假设我编写了一个 IndexBuffers 对象用于管理 EBO （将`VertexBuffers.cpp`的绑定目标改为`GL_ELEMENT_ARRAY_BUFFER`即可）

那么初始化的代码顺序应该是这样的

```
float vertices[] = {
    // pos        // color
    -0.5f, -0.5f, 0.0f,  1, 0, 0, // 0
     0.5f, -0.5f, 0.0f,  0, 1, 0, // 1
     0.5f,  0.5f, 0.0f,  0, 0, 1, // 2
    -0.5f,  0.5f, 0.0f,  1, 1, 0  // 3
};

unsigned int indices[] = {
    0, 1, 2,   // 第一个三角形
    2, 3, 0    // 第二个三角形
};

const VertexArrays VAO;
VAO.bind();

VertexBuffers VBO(vertices, sizeof(vertices));
IndexBuffers EBO(indices, sizeof(indices));

glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
glEnableVertexAttribArray(1);
```

绘制正方形时，使用`glDrawElements()`函数而不是`glDrawArrays()`函数：

```diff
- glDrawArrays(GL_TRIANGLES, 0, 3);
+ glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
```

## 实现绘制多顶点网格的程序

### 再次调整项目结构

移除 `VertexArrays` 和 `VertexBuffers` 类

新增 `Mesh` 类，可以根据顶点数据与索引数据绘制网格

`Mesh.cpp`

```
#include "Mesh.h"

#include <glad/glad.h>

Mesh::Mesh(const float *vertices, const GLsizei vertexSize, const unsigned int *indices, const GLsizei indexCount) :
    indexCount(indexCount) {
    // 1. VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // 2. VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertexSize, vertices, GL_STATIC_DRAW);

    // 3. EBO
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indexCount * sizeof(unsigned int)), indices,
                 GL_STATIC_DRAW);

    // 4. layout(location = 0) position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), static_cast<void *>(nullptr));
    glEnableVertexAttribArray(0);

    // 5. layout(location = 1) color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// 绘制 Mesh
void Mesh::draw() const {
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
}

// 析构函数释放 VAO,VBO,EBO
Mesh::~Mesh() {
    glDeleteBuffers(1, &ebo);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}
```

整理后的程序入口

`main.cpp`

```
#include "GlfwWindow.h"
#include "Mesh.h" // 新的 Mesh 对象
#include "ShaderProgram.h"

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

    glEnable(GL_DEPTH_TEST);
    glfwSwapInterval(0);

    // 设置顶点数据
    float vertices[] = {
            // pos        // color
            -0.5f, -0.5f, 0.0f, 1, 0, 0, // 0
            0.5f,  -0.5f, 0.0f, 0, 1, 0, // 1
            0.5f,  0.5f,  0.0f, 0, 0, 1, // 2
            -0.5f, 0.5f,  0.0f, 1, 1, 0 // 3
    };

    // 设置索引数据
    unsigned int indices[] = {
            0, 1, 2, // 第一个三角形
            2, 3, 0 // 第二个三角形
    };

    // 创建 Mesh 对象
    const Mesh mesh(vertices, sizeof(vertices), indices, sizeof(indices) / sizeof(unsigned int));

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    const int timeLoc = glGetUniformLocation(shader.id, "uTime");

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

        // 调用 draw() 方法直接绘制出 Mesh
        mesh.draw();

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}
```

通过修改`顶点数据`和`索引数据`即可绘制出任意网格
