#include "camera.h"

#include "GLFW/glfw3.h"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch) :
    Front(0.0f, 0.0f, -1.0f), Speed(5.0f), Sensitivity(0.1f), Zoom(45.0f) {
    Position = position;
    WorldUp = up;
    Yaw = yaw;
    Pitch = pitch;
    updateVectors();
}

glm::mat4 Camera::GetViewMatrix() const { return glm::lookAt(Position, Position + Front, Up); }

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

void Camera::updateVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

    Front = glm::normalize(front);
    Right = glm::normalize(glm::cross(Front, WorldUp));
    Up = glm::normalize(glm::cross(Right, Front));
}
