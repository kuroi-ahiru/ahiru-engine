#include "Scene.h"

void Scene::AddGameObject(std::shared_ptr<GameObject> gameObject) {
    gameObjects.push_back(gameObject);
}

void Scene::RemoveGameObject(std::shared_ptr<GameObject> gameObject) {
    gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), gameObject), gameObjects.end());
}

void Scene::Render() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Render();
        }
    }
}

void Scene::Update() {
    for (auto& gameObject : gameObjects) {
        if (gameObject) {
            gameObject->Update();
        }
    }
}
