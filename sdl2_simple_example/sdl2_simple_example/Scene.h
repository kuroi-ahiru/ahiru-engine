#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include <GL/glew.h>
#include "GameObject.h"
#include <glm/glm.hpp>

class Scene {
public:
    Scene();
    ~Scene();

    void AddGameObject(std::shared_ptr<GameObject> gameObject);
    void RemoveGameObject(std::shared_ptr<GameObject> gameObject);
    void Render();
    void Update();
    // Declaración de AddCube con posición opcional
    void AddCube(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));
    void AddSphere(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));
    void AddCone(const std::string& name, const glm::vec3& position = glm::vec3(0.0f));


    std::shared_ptr<GameObject> CreateGameObject(const char* modelFile, const char* textureFile);

    const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }
    std::shared_ptr<GameObject> GetSelectedGameObject() const { return selectedGameObject; }
    void SetSelectedGameObject(std::shared_ptr<GameObject> gameObject) { selectedGameObject = gameObject; }

    std::shared_ptr<GameObject> PickGameObject(const glm::vec3& rayOrigin, const glm::vec3& rayDir);

    bool LoadModel(const char* modelFile, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices);
    GLuint LoadTexture(const char* textureFile);
    void DrawGrid(int grid_size = 30, float grid_spacing = 1.5f);

    void SetDebugMode(bool enabled) { debugMode = enabled; }

    void CalculateFrustumPlanes(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::shared_ptr<GameObject> selectedGameObject = nullptr;

    bool debugMode = false; //flag para habilitar/deshabilitar el debug visual mouse ray
    glm::vec3 debugRayOrigin; // Origen del rayo
    glm::vec3 debugRayDir;    // Direccion del rayo

    struct FrustumPlane {
        glm::vec3 normal;
        float distance;
    };

    // Frustum culling
    FrustumPlane frustumPlanes[6];
    bool IsAABBInsideFrustum(const glm::vec3& min, const glm::vec3& max);

    glm::mat4 projectionMatrix;
    glm::mat4 viewMatrix;
};

#endif // SCENE_H