#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp> //para el render
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"


class GameObject {
public:
    GameObject(const std::string& name);
    ~GameObject();

    void AddComponent(std::shared_ptr<Component> component);

    void Update();

    void Render();

    void SetActive(bool active);
    bool IsActive() const;
    const std::string& GetName() const;

    //referencia const a la lista de componentes
    const std::vector<std::shared_ptr<Component>>& GetComponents() const;

    glm::vec3 GetBoundingBoxMin() const;
    glm::vec3 GetBoundingBoxMax() const;
    glm::vec3 GetPosition() const;


private:
    std::string name;
    bool active;
    bool isStatic;
    std::vector<std::shared_ptr<Component>> components;
};

#endif // GAMEOBJECT_H
