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

    std::shared_ptr<GameObject> CreateGameObject(const char* modelFile, const char* textureFile);

    const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const { return gameObjects; }
    // Métodos para gestionar el GameObject seleccionado
    std::shared_ptr<GameObject> GetSelectedGameObject() const { return selectedGameObject; }
    void SetSelectedGameObject(std::shared_ptr<GameObject> gameObject) { selectedGameObject = gameObject; }

    bool LoadModel(const char* modelFile, std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& texCoords, std::vector<unsigned int>& indices);
    GLuint LoadTexture(const char* textureFile);
    void DrawGrid(int grid_size = 30, float grid_spacing = 1.5f);

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
    std::shared_ptr<GameObject> selectedGameObject = nullptr;
};

#endif // SCENE_H