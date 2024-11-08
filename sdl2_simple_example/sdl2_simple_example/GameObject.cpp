// GameObject.cpp
#include "GameObject.h"
#include <algorithm>

GameObject::GameObject(const std::string& name) : name(name), active(true) {}

GameObject::~GameObject() {}

void GameObject::AddComponent(std::shared_ptr<Component> component) {
    components.push_back(component);
}

void GameObject::Update() {
    if (active) {
        for (auto& component : components) {
            if (component->IsActive()) {
                component->Update();
            }
        }
    }
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
