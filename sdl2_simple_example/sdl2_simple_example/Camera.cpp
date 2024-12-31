#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // Para proyecciones   
#include <cmath>

Camera::Camera(const glm::vec3& position, const glm::vec3& target, float fov, float aspectRatio, float nearPlane, float farPlane)
    : position(position), worldUp(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f),
    fov(fov), aspectRatio(aspectRatio), nearPlane(nearPlane), farPlane(farPlane),
    isRightButtonPressed(false), lastMouseX(0), lastMouseY(0) {
    front = glm::normalize(target - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up = glm::cross(right, front);
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::Update(float deltaTime, const Uint8* keyboardState, int mouseX, int mouseY, Uint32 mouseState) {
    const float sensitivity = 0.1f;
    const float cameraSpeed = 5.0f * deltaTime;

    if (mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {
        if (!isRightButtonPressed) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            isRightButtonPressed = true;
        }

        float xOffset = mouseX - lastMouseX;
        float yOffset = lastMouseY - mouseY;

        xOffset *= sensitivity;
        yOffset *= sensitivity;

        yaw += xOffset;
        pitch += yOffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(direction);

        lastMouseX = mouseX;
        lastMouseY = mouseY;
    }
    else {
        isRightButtonPressed = false;
    }

    if (keyboardState[SDL_SCANCODE_W]) position += cameraSpeed * front;
    if (keyboardState[SDL_SCANCODE_S]) position -= cameraSpeed * front;
    if (keyboardState[SDL_SCANCODE_A]) position -= cameraSpeed * right;
    if (keyboardState[SDL_SCANCODE_D]) position += cameraSpeed * right;

    UpdateViewMatrix();
}

glm::vec3 Camera::CalculateRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const {
    float x = (2.0f * mouseX) / screenWidth - 1.0f;
    float y = 1.0f - (2.0f * mouseY) / screenHeight;
    float z = 1.0f;

    glm::vec4 rayClip(x, y, -1.0f, 1.0f);
    glm::vec4 rayEye = glm::inverse(projectionMatrix) * rayClip;
    rayEye.z = -1.0f;
    rayEye.w = 0.0f;

    glm::vec3 rayWorld = glm::vec3(glm::inverse(viewMatrix) * rayEye);
    return glm::normalize(rayWorld);
}

void Camera::UpdateViewMatrix() {
    viewMatrix = glm::lookAt(position, position + front, up);
}

void Camera::UpdateProjectionMatrix() {
    projectionMatrix = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
}
