#ifndef COMPONENTTRANSFORM_H
#define COMPONENTTRANSFORM_H

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class ComponentTransform : public Component {
public:
    ComponentTransform(GameObject* owner, const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
        : Component(Type::Transform, owner), position(position), rotation(rotation), scale(scale) {}

    void Update() override {}
    void Render() override {}

    // Getters para el inspector
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetRotation() const { return rotation; }
    const glm::vec3& GetScale() const { return scale; }

    // Setters, en caso de que el editor permita editar transformaciones
    void SetPosition(const glm::vec3& newPos) { position = newPos; }
    void SetRotation(const glm::vec3& newRot) { rotation = newRot; }
    void SetScale(const glm::vec3& newScale) { scale = newScale; }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

#endif // COMPONENTTRANSFORM_H

