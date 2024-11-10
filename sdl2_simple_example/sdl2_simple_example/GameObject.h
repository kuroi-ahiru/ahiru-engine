#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "Component.h"

class GameObject {
public:
    GameObject(const std::string& name) : name(name), active(true) {} // Definición en línea del constructor, revisar si mejor separar en h y cpp
    ~GameObject() = default;

    void AddComponent(std::shared_ptr<Component> component) {
        components.push_back(component);
    }

    void Update() {
        if (active) {
            for (auto& component : components) {
                if (component) component->Update();
            }
        }
    }

    void Render() {
        for (auto& component : components) {
            if (component) component->Render();
        }
    }

    void SetActive(bool active) { this->active = active; }
    bool IsActive() const { return active; }
    const std::string& GetName() const { return name; }

    //referencia const a la lista de componentes
    const std::vector<std::shared_ptr<Component>>& GetComponents() const {
        return components;
    }

private:
    std::string name;
    bool active;
    std::vector<std::shared_ptr<Component>> components;
};

#endif // GAMEOBJECT_H
