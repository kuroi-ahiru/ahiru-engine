#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include <glm/gtc/type_ptr.hpp> //para el render


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

        // buscar el componente de transformacion
        for (auto& component : components) {
            if (component && component->GetType() == Component::Type::Transform) {
                auto* transformComponent = dynamic_cast<ComponentTransform*>(component.get());
                if (transformComponent) {
                    transform = transformComponent->GetTransformMatrix();
                    break;
                }
            }
        }

        // aplicar transformacion en OpenGL
        glPushMatrix();
        glMultMatrixf(glm::value_ptr(transform));
                
        for (auto& component : components) {
            if (component) component->Render();
        }

        glPopMatrix();
    }

    void SetActive(bool active) { this->active = active; }
    bool IsActive() const { return active; }
    const std::string& GetName() const { return name; }

    //referencia const a la lista de componentes
    const std::vector<std::shared_ptr<Component>>& GetComponents() const {
        return components;
    }

    glm::vec3 GetBoundingBoxMin() const {
        for (const auto& component : components) {
            if (component && component->GetType() == Component::Type::Mesh) {
                auto* meshComponent = dynamic_cast<ComponentMesh*>(component.get());
                if (meshComponent) {
                    return meshComponent->GetBoundingBoxMin();
                }
            }
        }
        return glm::vec3(0.0f); // Valor por defecto si no hay un componente de malla
    }

    glm::vec3 GetBoundingBoxMax() const {
        for (const auto& component : components) {
            if (component && component->GetType() == Component::Type::Mesh) {
                auto* meshComponent = dynamic_cast<ComponentMesh*>(component.get());
                if (meshComponent) {
                    return meshComponent->GetBoundingBoxMax();
                }
            }
        }
        return glm::vec3(0.0f); // Valor por defecto si no hay un componente de malla
    }

    glm::vec3 GetPosition() const {
        for (const auto& component : components) {
            if (component && component->GetType() == Component::Type::Transform) {
                auto* transformComponent = dynamic_cast<ComponentTransform*>(component.get());
                if (transformComponent) {
                    return transformComponent->GetPosition();
                }
            }
        }
        return glm::vec3(0.0f); // Valor por defecto si no hay un componente de transformacion
    }


private:
    std::string name;
    bool active;
    bool isStatic;
    std::vector<std::shared_ptr<Component>> components;
};

#endif // GAMEOBJECT_H
