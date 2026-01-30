# OpenGL 学习笔记（键鼠交互与摄像机）

## 键鼠交互

### deltaTime

每次渲染循环的时间，必须先计算`deltaTime`才能保证键鼠操作均匀

循环前初始化

```
float deltaTime = 0.0f;
float lastFrame = 0.0f;
```

然后在每次循环内

```
const auto current = static_cast<float>(glfwGetTime());
deltaTime = current - lastFrame;
lastFrame = current;
```

### 键盘

```
if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
    ...
}
```

`GLFW_KEY_A`对应 `A` 键，`GLFW_PRESS`代表按下，同理可以接受别的按键

> 如果使用此仓库代码的 `GlfwWindow` 对象，请对 `windows` 调用 `.as_ptr()` 方法

### 鼠标

创建鼠标回调函数

```
void mouse_callback(GLFWwindow *, double xpos, double ypos) {
    static float lastX = 960;
    static float lastY = 540;
    static bool firstMouse = true;

    if (firstMouse) {
        lastX = static_cast<float>(xpos);
        lastY = static_cast<float>(ypos);
        firstMouse = false;
    }

    float xoffset = static_cast<float>(xpos) - lastX;
    auto yoffset = static_cast<float>(lastY - ypos); // Y 轴反向
    lastX = static_cast<float>(xpos);
    lastY = static_cast<float>(ypos);

    // 此处得到 xoffset, yoffset 即为鼠标位移
}
```

注册回调函数

```
glfwSetCursorPosCallback(window, mouse_callback);
glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
```

> 如果使用此仓库代码的 `GlfwWindow` 对象，请对 `windows` 调用 `.as_ptr()` 方法

## 相机

### 创建 `Camera` 类

> 此处省略 `Camera.h` （见仓库）

`Camera.cpp`

|    成员变量     |               作用               |
|:-----------:|:------------------------------:|
|  Position   |             摄像机位置              |
|    Front    |             摄像机朝向              |
|  Right/Up   | 通过 Front 计算得到，分别用于 View 和 左右移动 |
|   WorldUp   |         世界的上方，防止摄像机翻转          |
|     Yaw     |              水平角度              |
|    Pitch    |              垂直角度              |
|    Speed    |              移动速度              |
| Sensitivity |             鼠标灵敏度              |
|    Zoom     |              透视强弱              |

```
#include "camera.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    Front(0.0f, 0.0f, -1.0f), Speed(5.0f), Sensitivity(0.08f), Zoom(45.0f) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateVectors();
}

// 在 Camera 对象内计算 View 矩阵
glm::mat4 Camera::GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

// 键盘
void Camera::ProcessKeyboard(const int key, const float deltaTime) {
    float v = Speed * deltaTime;
    if (key == GLFW_KEY_W)
        Position += Front * v;
    if (key == GLFW_KEY_S)
        Position -= Front * v;
    if (key == GLFW_KEY_A)
        Position -= Right * v;
    if (key == GLFW_KEY_D)
        Position += Right * v;
}

// 鼠标
void Camera::ProcessMouse(float xoffset, float yoffset) {
    xoffset *= Sensitivity;
    yoffset *= Sensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (Pitch > 89.0f)
        Pitch = 89.0f;
    if (Pitch < -89.0f)
        Pitch = -89.0f;

    updateVectors();
}

// 更新 Front Right Up
void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
```

### 修改 `main.cpp`

```
#include "GlfwWindow.h"
#include "Mesh.h"
#include "ShaderProgram.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"

int main() {
    ... 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    const int timeLoc = glGetUniformLocation(shader.id, "uTime");

    // ----- 上面的内容无变化 -----

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

        // 无变化
        const auto timeValue = static_cast<float>(glfwGetTime());
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Model 无变化
        auto model = glm::mat4(1.0f);
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

        // View 调用 camera 的 GetViewMatrix() 方法计算
        glm::mat4 view = camera.GetViewMatrix();
        
        // Projection 略有变化
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), 1920.0f / 1080.0f, 0.1f, 100.0f);


        // ----- 下面的内容无变化 -----
        glm::mat4 mvp = projection * view * model;

        const int mvpLoc = glGetUniformLocation(shader.id, "uMVP");
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        shader.use();
        glUniform1f(timeLoc, timeValue);

        mesh.draw();

        glfwSwapBuffers(window.as_ptr());
        glfwPollEvents();
    }
}

```

运行程序，摄像机能根据键盘 WASD 输入移动，根据鼠标移动改变观察方向