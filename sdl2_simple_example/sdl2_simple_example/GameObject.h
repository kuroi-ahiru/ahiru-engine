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

    void CalculateAABB();
    void DrawAABB(const glm::vec3& color = glm::vec3(0.0f, 1.0f, 0.0f));  //(debug)
    const glm::vec3& GetAABBMin() const { return aabbMin; } //mirar si puc resumir en 1 aquests gett amb els de dalt
    const glm::vec3& GetAABBMax() const { return aabbMax; }


private:
    std::string name;
    bool active;
    bool isStatic;
    std::vector<std::shared_ptr<Component>> components;

    //calculo AABB
    glm::vec3 aabbMin; //esquina min de AABB
    glm::vec3 aabbMax; //esquina max de AABB
};

#endif // GAMEOBJECT_H
