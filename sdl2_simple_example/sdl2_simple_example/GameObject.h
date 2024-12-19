#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "Component.h"
#include <glm/gtc/type_ptr.hpp> //para el render

class ComponentTransform; //declaracion anticipada ?? para los errores pero luego me lo miro

class GameObject {
public:
    GameObject(const std::string& name) : name(name), active(true) {}
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
        /*for (auto& component : components) {
            if (component) component->Render();
        }*/

        if (!active) return;

        glm::mat4 transform = glm::mat4(1.0f);

        // Buscar el componente de transformación
        for (auto& component : components) {
            if (component && component->GetType() == Component::Type::Transform) {
                auto* transformComponent = dynamic_cast<ComponentTransform*>(component.get());
                if (transformComponent) {
                    transform = transformComponent->GetTransformMatrix();
                    break;
                }
            }
        }

        // Aplicar la transformación en OpenGL
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(transform));

        // Renderizar todos los componentes
        for (auto& component : components) {
            if (component) component->Render();
        }

        glPopMatrix(); // Restaurar la matriz
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
    bool isStatic;
    std::vector<std::shared_ptr<Component>> components;
};

#endif // GAMEOBJECT_H
