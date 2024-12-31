#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov, float aspect, float nearPlane, float farPlane);

    void updateCamera(float deltaTime);  // Actualiza la cámara según la entrada del usuario
    void processKeyboardInput(const uint8_t* state, float deltaTime);  // Procesa la entrada del teclado
    void ProcessMouseInput(int mouseX, int mouseY, bool rightButtonPressed, float deltaTime);  // Procesa la entrada del ratón
    glm::mat4 getViewMatrix() const;  // Devuelve la matriz de vista
    glm::mat4 getProjectionMatrix() const;  // Devuelve la matriz de proyección

private:
    void UpdateCameraVectors();  // Actualiza los vectores de la cámara

    glm::vec3 m_position;  // Posición de la cámara
    glm::vec3 m_front;  // Dirección hacia donde apunta la cámara
    glm::vec3 m_up;  // Vector 'arriba' de la cámara
    glm::vec3 m_right;  // Vector 'derecha' de la cámara

    float m_yaw, m_pitch;  // Ángulos de la cámara
    float m_fov;  // Ángulo de visión (FOV)
    float m_speed;  // Velocidad de la cámara
    float m_sensitivity;  // Sensibilidad de la cámara
    float m_lastMouseX, m_lastMouseY;  // Últimas coordenadas del ratón

    glm::mat4 m_projectionMatrix;  // Matriz de proyección
    glm::mat4 m_viewMatrix;  // Matriz de vista
};

#endif // CAMERA_H
