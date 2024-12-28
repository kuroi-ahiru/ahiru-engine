#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp> //para OpenGL



GameObject::GameObject(const std::string& name)
    : name(name), active(true), isStatic(false) {}

GameObject::~GameObject() = default;

void GameObject::AddComponent(std::shared_ptr<Component> component) {
    components.push_back(component);
}

void GameObject::Update() {
    if (active) {
        for (auto& component : components) {
            if (component) component->Update();
        }
    }
}

void GameObject::Render() {
    if (!active) return;

    glm::mat4 transform = glm::mat4(1.0f);

    //buscar componente de transformacion
    for (auto& component : components) {
        if (component && component->GetType() == Component::Type::Transform) {
            auto* transformComponent = dynamic_cast<ComponentTransform*>(component.get());
            if (transformComponent) {
                transform = transformComponent->GetTransformMatrix();
                break;
            }
        }
    }

    //aplicar transformacion en OpenGL
    glPushMatrix();
    glMultMatrixf(glm::value_ptr(transform));

    for (auto& component : components) {
        if (component) component->Render();
    }

    glPopMatrix();
}

void GameObject::SetActive(bool active) {
    this->active = active;
}

bool GameObject::IsActive() const {
    return active;
}

const std::string& GameObject::GetName() const {
    return name;
}

const std::vector<std::shared_ptr<Component>>& GameObject::GetComponents() const {
    return components;
}

glm::vec3 GameObject::GetBoundingBoxMin() const {
    for (const auto& component : components) {
        if (component && component->GetType() == Component::Type::Mesh) {
            auto* meshComponent = dynamic_cast<ComponentMesh*>(component.get());
            if (meshComponent) {
                return meshComponent->GetBoundingBoxMin();
            }
        }
    }
    return glm::vec3(0.0f); //valor por defecto si no hay mesh component
}

glm::vec3 GameObject::GetBoundingBoxMax() const {
    for (const auto& component : components) {
        if (component && component->GetType() == Component::Type::Mesh) {
            auto* meshComponent = dynamic_cast<ComponentMesh*>(component.get());
            if (meshComponent) {
                return meshComponent->GetBoundingBoxMax();
            }
        }
    }
    return glm::vec3(0.0f); //valor por defecto si no hay mesh component
}

glm::vec3 GameObject::GetPosition() const {
    for (const auto& component : components) {
        if (component && component->GetType() == Component::Type::Transform) {
            auto* transformComponent = dynamic_cast<ComponentTransform*>(component.get());
            if (transformComponent) {
                return transformComponent->GetPosition();
            }
        }
    }
    return glm::vec3(0.0f); //valor por defecto si no hay transform component
}
