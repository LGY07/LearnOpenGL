#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 Position{};
    glm::vec3 Front;
    glm::vec3 Up{};
    glm::vec3 Right{};
    glm::vec3 WorldUp{};

    float Yaw;
    float Pitch;
    float Speed;
    float Sensitivity;
    float Zoom;

    explicit Camera(glm::vec3 position = {0.0f, 0.0f, 3.0f}, glm::vec3 up = {0.0f, 1.0f, 0.0f}, float yaw = -90.0f,
                    float pitch = 0.0f);

    [[nodiscard]] glm::mat4 GetViewMatrix() const;
    void ProcessKeyboard(int key, float deltaTime);
    void ProcessMouse(float xoffset, float yoffset);

private:
    void updateVectors();
};
