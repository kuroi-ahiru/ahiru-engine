#include "Camera.h"
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <SDL2/SDL.h>

Camera::Camera(float fov, float aspect, float nearPlane, float farPlane)
    : m_position(0.0f, 0.0f, 5.0f), m_front(0.0f, 0.0f, -1.0f), m_up(0.0f, 1.0f, 0.0f),
    m_right(1.0f, 0.0f, 0.0f), m_yaw(-90.0f), m_pitch(0.0f), m_fov(fov), m_speed(2.5f), m_sensitivity(0.1f) {
    m_projectionMatrix = glm::perspective(glm::radians(fov), aspect, nearPlane, farPlane);
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}


void Camera::updateCamera(float deltaTime) {
    // Este método puede usarse para actualizar matrices o cualquier otro comportamiento
    m_viewMatrix = glm::lookAt(m_position, m_position + m_front, m_up);
}

void Camera::processKeyboardInput(const uint8_t* state, float deltaTime) {
    // Movimiento hacia adelante y hacia atrás
    if (state[SDL_SCANCODE_W]) {
        m_position += m_front * m_speed * deltaTime;
    }
    if (state[SDL_SCANCODE_S]) {
        m_position -= m_front * m_speed * deltaTime;
    }

    // Movimiento hacia la izquierda y hacia la derecha
    if (state[SDL_SCANCODE_A]) {
        m_position -= m_right * m_speed * deltaTime;
    }
    if (state[SDL_SCANCODE_D]) {
        m_position += m_right * m_speed * deltaTime;
    }

    // Movimiento hacia arriba y hacia abajo
    if (state[SDL_SCANCODE_Q]) {
        m_position += m_up * m_speed * deltaTime;
    }
    if (state[SDL_SCANCODE_E]) {
        m_position -= m_up * m_speed * deltaTime;
    }

    // Actualiza los vectores de la cámara con la nueva posición
    UpdateCameraVectors();
}




void Camera::ProcessMouseInput(int mouseX, int mouseY, bool rightButtonPressed, float deltaTime) {
    if (rightButtonPressed) {
        // Calculamos el desplazamiento del ratón desde el último evento
        float xOffset = mouseX - m_lastMouseX;
        float yOffset = m_lastMouseY - mouseY;  // Invertimos Y

        xOffset *= m_sensitivity;
        yOffset *= m_sensitivity;

        m_yaw += xOffset;
        m_pitch += yOffset;

        // Limitar la inclinación (pitch) para evitar que se dé vuelta la cámara
        if (m_pitch > 89.0f) m_pitch = 89.0f;
        if (m_pitch < -89.0f) m_pitch = -89.0f;

        UpdateCameraVectors();
    }

    // Actualizamos las posiciones del ratón
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;
}


void Camera::UpdateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));

    m_front = glm::normalize(front);
    m_right = glm::normalize(glm::cross(m_front, m_up));  // Right vector
}


glm::mat4 Camera::getViewMatrix() const {
    return m_viewMatrix;
}

glm::mat4 Camera::getProjectionMatrix() const {
    return m_projectionMatrix;
}