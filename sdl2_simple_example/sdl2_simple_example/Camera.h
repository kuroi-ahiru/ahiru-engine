#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(float fov, float aspect, float nearPlane, float farPlane);

    void updateCamera(float deltaTime);  // Actualiza la c�mara seg�n la entrada del usuario
    void processKeyboardInput(const uint8_t* state, float deltaTime);  // Procesa la entrada del teclado
    void ProcessMouseInput(int mouseX, int mouseY, bool rightButtonPressed, float deltaTime);  // Procesa la entrada del rat�n
    glm::mat4 getViewMatrix() const;  // Devuelve la matriz de vista
    glm::mat4 getProjectionMatrix() const;  // Devuelve la matriz de proyecci�n

private:
    void UpdateCameraVectors();  // Actualiza los vectores de la c�mara

    glm::vec3 m_position;  // Posici�n de la c�mara
    glm::vec3 m_front;  // Direcci�n hacia donde apunta la c�mara
    glm::vec3 m_up;  // Vector 'arriba' de la c�mara
    glm::vec3 m_right;  // Vector 'derecha' de la c�mara

    float m_yaw, m_pitch;  // �ngulos de la c�mara
    float m_fov;  // �ngulo de visi�n (FOV)
    float m_speed;  // Velocidad de la c�mara
    float m_sensitivity;  // Sensibilidad de la c�mara
    float m_lastMouseX, m_lastMouseY;  // �ltimas coordenadas del rat�n

    glm::mat4 m_projectionMatrix;  // Matriz de proyecci�n
    glm::mat4 m_viewMatrix;  // Matriz de vista
};

#endif // CAMERA_H
