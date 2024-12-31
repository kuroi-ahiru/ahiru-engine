#ifndef CAMERA_H
#define CAMERA_H

#define SDL_MAIN_HANDLED //nose una fumada muy rara pero si no pongo esto se me redefine el main ya lo mirare
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL2/SDL.h> // Para tipos de eventos y ratón

class Camera {
public:
    Camera(const glm::vec3& position, const glm::vec3& target, float fov, float aspectRatio, float nearPlane, float farPlane);

    void Update(float deltaTime, const Uint8* keyboardState, int mouseX, int mouseY, Uint32 mouseState);
    glm::vec3 CalculateRay(int mouseX, int mouseY, int screenWidth, int screenHeight) const;

    const glm::mat4& GetViewMatrix() const { return viewMatrix; }
    const glm::mat4& GetProjectionMatrix() const { return projectionMatrix; }
    const glm::vec3& GetPosition() const { return position; }

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float fov;
    float aspectRatio;
    float nearPlane;
    float farPlane;

    glm::mat4 viewMatrix;
    glm::mat4 projectionMatrix;

    bool isRightButtonPressed;
    int lastMouseX, lastMouseY;
};

#endif // CAMERA_H
