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
