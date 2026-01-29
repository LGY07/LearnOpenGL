# OpenGL 学习笔记（创建窗口并绘制三角形）

## 配置环境

> 已经在 Windows 配置好了 `CMake 4.1.2` `Ninja 1.13.2` `Clang 21.1.0(LLVM 21.1.0)`
>
> 使用 clang-cl 作为 C/C++ 编译器
>
> 省略 C/C++ 编译器的配置部分

> 使用的 OpenGL 版本为 OpenGL 4.6 Core
>
> 使用的 C++ 标准为 C++26

### 创建项目

先创建以下目录和文件

```
LearnOpenGL/
├─ main.cpp         <- 程序入口
├─ CMakeLists.txt   <- CMake 配置
├─ shaders/         <- 用 GLSL 写的着色器
│  ├─ basic.vert    <- 顶点着色器
│  ├─ basic.frag    <- 片段着色器
│  └─ common.glsl
├─ src /
├─ include/
├─ third_party/
│  ├─ glfw/         <- GLFW 库（窗口）
│  ├─ glad/         <- GLAD 库（函数加载）
│  └─ glm/          <- GLM 库（数学）

```

### 准备 GLFW

前往 [GLFW 主页](https://www.glfw.org/) 下载 GLFW 的源码压缩包，得到 `glfw-3.4.zip`
并解压（也可以在[下载页面](https://www.glfw.org/download.html)获取预编译的 GLFW 库，建议从源码编译提高成功率）

进入解压后的 `glfw-3.4` 目录编译

```
mkdir build
cd build
cmake .. -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl
cmake --build .
```

如果没有出错，你会在 `glfw-3.4/build/src` 目录得到 `glfw3.lib` 文件

在项目目录的 `third_party/glfw` 下创建 `lib` 目录，然后把 `glfw3.lib` 放进去

最后把整个 `glfw-3.4/include` 复制到 `third_party/glfw`

```
third_party/
├─ glfw/
│  ├─ lib/
│  │  └─ glfw3.lib
│  └─ include/
```

### 准备 GLAD

打开 [GLAD 下载页面](https://glad.dav1d.de/) 填写选项：

* Language: C/C++
* Specification: OpenGL
* API: gl version 4.6
* Profile: Core

最后点击 GENERATE 下载 `glad.zip`

解压后整个目录内容（`src`和`include`）放到 `third_party/glad`

### 准备 GLM

从 [Github 仓库](https://github.com/g-truc/glm/releases/latest)下载 GLM

解压后整个目录的内容放到 `third_party/glm`

### 配置 CMake

编辑 `CMakeLists.txt`

```
cmake_minimum_required(VERSION 4.0)
project(LearnOpenGL)

set(CMAKE_CXX_STANDARD 26)

add_executable(LearnOpenGL WIN32
        main.cpp
        third_party/glad/src/glad.c
)

target_link_options(LearnOpenGL PRIVATE
        -Wl,/SUBSYSTEM:WINDOWS
        -Wl,/ENTRY:mainCRTStartup
)

target_include_directories(LearnOpenGL PRIVATE
        third_party/glad/include
        third_party/glfw/include
        third_party/glm
)

target_link_directories(LearnOpenGL PRIVATE
        third_party/glfw/lib
)

target_link_libraries(LearnOpenGL PRIVATE
        glfw3
        opengl32
)
```

### 检查配置

如果配置正确，以下代码能创建一个灰色的窗口

```
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

int main() {
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(1920, 1080, "Hello OpenGL", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
}
```

> 注意：
>
> #include <glad/glad.h>
>
> #include <GLFW/glfw3.h>
>
> 必须严格按照顺序 include，根据字母顺序，某些格式化工具会把 glfw 放到 glad 前面，导致无法编译，可以 在 glad 和 glfw
> 间插入空行来避免排序

## 创建第一个三角形

* VAO：顶点数组对象

  通过 VAO 配置着色器如何读取 VBO

* VBO：顶点缓冲对象

  GPU 上的一块显存

> 主要内容在注释

```
#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <iostream>

// 顶点着色器
const char *vertexShaderSrc = R"(
#version 460 core // OpenGL 版本
// in 关键字初始化输入的变量
// out 关键字初始化输出的变量
// layout (location = x) 根据 VAO 的配置捕获数据
layout (location = 0) in vec3 aPos; // 按照 VOA 中的配置，此处会分 3 次获得三角形的顶点位置
void main() {
    gl_Position = vec4(aPos, 1.0); // 给 gl_Position 赋值把位置直接交给 OpenGL
}
)";

// 片段着色器
const char *fragmentShaderSrc = R"(
#version 460 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0); // 给顶点设置颜色
}
)";


int main() {
    // 初始化 GLFW，设置 OpenGL 版本为 4.6
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 初始化窗口，宽 1920 高 1080 标题 "Hello OpenGL"
    GLFWwindow *window = glfwCreateWindow(1920, 1080, "Hello OpenGL", nullptr, nullptr);
    if (!window)
        return -1;
    glfwMakeContextCurrent(window);
    // 初始化 GLAD
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to init GLAD\n";
        return -1;
    }

    // 编译着色器
    auto compile = [](GLenum type, const char *src) { // 编译着色器的闭包
        unsigned int s = glCreateShader(type); // 创建着色器
        glShaderSource(s, 1, &src, nullptr); // 设置着色器源码
        glCompileShader(s); // 执行编译
        return s;
    };

    const unsigned int vs = compile(GL_VERTEX_SHADER, vertexShaderSrc); // 编译顶点着色器
    const unsigned int fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSrc); // 编译片段着色器

    const unsigned int shaderProgram = glCreateProgram(); // 创建着色程序
    glAttachShader(shaderProgram, vs); // 添加顶点着色器到着色程序
    glAttachShader(shaderProgram, fs); // 添加片段着色器到着色程序
    glLinkProgram(shaderProgram); // 链接着色程序
    // 清理
    glDeleteShader(vs);
    glDeleteShader(fs);

    // 初始化 VAO 和 VBO
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 将要上传到 VBO 的数据
    constexpr float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

    // 绑定 VAO 和 VBO，绑定顺序一定是 VAO,VBO,EBO
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 将数据复制到 VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 配置 VAO
    glVertexAttribPointer(0, // layout (location = 0)
                          3, // 要捕获的元素格数
                          GL_FLOAT, // 要捕获的元素类型
                          GL_FALSE, // 是否归一化
                          3 * sizeof(float), // 每组数据大小
                          nullptr // 要捕获的数据在每组数据内的起始位置
    );
    glEnableVertexAttribArray(0); // 启用 layout (location = 0)

    // 渲染循环
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // 设置背景颜色
        glClear(GL_COLOR_BUFFER_BIT); // 清理

        glUseProgram(shaderProgram); // 使用着色器
        glBindVertexArray(VAO); // 绑定 VAO，每次循环都执行以避免多个 VAO 时出错
        glDrawArrays(GL_TRIANGLES, 0, 3); // 画三角形

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // 释放资源
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
}
```