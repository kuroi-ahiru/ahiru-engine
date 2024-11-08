#include "Component.h"

Component::Component(Type type, GameObject* owner)
    : type(type), owner(owner), active(true) {}

void Component::Enable() {
    active = true;
}

void Component::Disable() {
    active = false;
}

bool Component::IsActive() const {
    return active;
}

void Component::Update() {
    // Comportamiento básico para los componentes
}
