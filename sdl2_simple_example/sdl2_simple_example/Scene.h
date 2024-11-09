#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "GameObject.h"

class Scene {
public:
    Scene() = default;
    ~Scene() = default;

    void AddGameObject(std::shared_ptr<GameObject> gameObject);
    void RemoveGameObject(std::shared_ptr<GameObject> gameObject);
    void Render();
    void Update();

private:
    std::vector<std::shared_ptr<GameObject>> gameObjects;
};

#endif // SCENE_H
