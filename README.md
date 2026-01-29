# OpenGL 学习笔记（三角形的颜色与插值）

## 重新整理项目结构

将着色器拆分至 `shaders` 目录，程序会在运行时读取并编译着色器

将 OpenGL 初始化、着色器编译、VAO 和 VBO 的管理写为四个对象拆分至 `include` 和 `src` 目录，此处为 C++ 的内容不描述，具体可见仓库源码

## 三角形的渐变与呼吸效果

### 顶点着色器 `basic.vert`

```
#version 460 core

layout (location = 0) in vec3 aPos; // 输入位置
layout (location = 1) in vec3 aColOff; // 输入颜色

out vec3 vColOff; // 输出颜色

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColOff = aColOff; // 颜色传递给片段着色器
}
```

### 片段着色器 `basic.frag`

```
#version 460 core

in vec3 vColOff;
out vec4 FragColor;

uniform float uTime; // 初始化统一变量

void main()
{
    // 计算颜色
    vec3 color = vec3(
    sin(uTime + vColOff.x),
    sin(uTime + vColOff.y + 2.0),
    sin(uTime + vColOff.z + 4.0)
    ) * 0.5 + 0.5;

    // 输出颜色
    FragColor = vec4(color, 1.0);
}
```

### 以下为重新整理后的样板代码

```
#include "GlfwWindow.h"
#include "ShaderProgram.h"
#include "VertexArrays.h"
#include "VertexBuffers.h"

int main() {
    // 初始化 OpenGL
    const GlfwWindow window(1920, 1080, "Hello OpenGL");

    // 编译链接着色器
    const ShaderProgram shader;
    shader.attach(GL_VERTEX_SHADER, "../shaders/basic.vert");
    shader.attach(GL_FRAGMENT_SHADER, "../shaders/basic.frag");
    shader.link();
    shader.use();

    【需要注意的部分 1】

    // 初始化 VAO、VBO
    const VertexArrays VAO;
    VAO.bind();
    VertexBuffers VBO(vertices, sizeof(vertices));

    【需要注意的部分 2】
    
    【需要注意的部分 3】

}
```

### 【需要注意的部分 1】

```
float vertices[] = {
            // pos              // color
            -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 顶点 1
            0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 顶点 2
            0.0f,  0.5f,  0.0f, 0.0f, 0.0f, 1.0f // 顶点 3
    };
```

此处上传给 VBO 的顶点数据与第一章不同，新增了颜色信息

一共有 3 个顶点，每个顶点有位置和颜色 2 种数据，两种都包含 3 个元素，顶点着色器分别尝试从`layout (location = 0)`和
`layout (location = 1)`来获取两种数据

### 【需要注意的部分 2】

```
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
```

VBO 的格式改变了，这里修改 VAO 让着色器正确读取 VBO 的数据

`glVertexAttribPointer(index, size, type, normalized, stride, pointer)` 函数的参数的含义如下

|     参数     |                    含义                    |
|:----------:|:----------------------------------------:|
|   index    | 着色器从 layout (location = index) 的位置获取这种数据 |
|    size    |           这种数据有多少个元素（3对应 vec3）           |
|    type    |                 每个元素的类型                  |
| normalized |                  是否归一化                   |
|   stride   |               一个顶点总的有多大的数据               |
|  pointer   |             这种数据从顶点数据的什么位置开始             |

以 position 为例，在处理一个顶点时，着色器会从 VBO 读取`6 * sizeof(float)`大小的内存为一个顶点的数据，
从 nullptr 开始的 3 个 float 元素被放在 `layout (location = 0)`

### 【需要注意的部分 3】

```
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
```

* 统一变量 uniform

片段着色器中有以下语句：

`uniform float uTime;`

`glGetUniformLocation()`函数获取了着色器中名为`uTime`的统一变量的位置`timeLoc`

每次循环都会获取时间并传递给统一变量

片段着色器对时间和偏移量求正弦值作为颜色，实现呼吸效果

### 插值与渐变

默认情况下，显卡会自动完成插值，在三个顶点设置不同的颜色即可实现渐变效果

通过 `flat` 关键字可以强制禁用插值，显卡将会以最后一个顶点作为显示颜色

`basic.vert`

```
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColOff;

flat out vec3 vColOff;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    vColOff = aColOff;
}
```

`basic.frag`

```
#version 460 core

flat in vec3 vColOff;
out vec4 FragColor;

uniform float uTime;

void main()
{
    vec3 color = vec3(
    sin(uTime + vColOff.x),
    sin(uTime + vColOff.y + 2.0),
    sin(uTime + vColOff.z + 4.0)
    ) * 0.5 + 0.5;

    FragColor = vec4(color, 1.0);
}
```