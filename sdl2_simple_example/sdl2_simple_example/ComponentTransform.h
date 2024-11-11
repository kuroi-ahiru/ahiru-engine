#ifndef COMPONENTTRANSFORM_H
#define COMPONENTTRANSFORM_H

#include "Component.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

class ComponentTransform : public Component {
public:
    ComponentTransform(GameObject* owner, const glm::vec3& position = glm::vec3(0.0f),
        const glm::vec3& rotation = glm::vec3(0.0f),
        const glm::vec3& scale = glm::vec3(1.0f))
        : Component(Type::Transform, owner), position(position), rotation(rotation), scale(scale) {
        UpdateTransformMatrix();
    }

    void Update() override {}
    void Render() override {}

    // Getters
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetRotation() const { return rotation; }
    const glm::vec3& GetScale() const { return scale; }
    const glm::mat4& GetTransformMatrix() const { return transformMatrix; }  // Para acceder a la matriz de transformación

    // Setters con actualización de la matriz
    void SetPosition(const glm::vec3& newPos) {
        position = newPos;
        UpdateTransformMatrix();
    }
    void SetRotation(const glm::vec3& newRot) {
        rotation = newRot;
        UpdateTransformMatrix();
    }
    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        UpdateTransformMatrix();
    }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
    glm::mat4 transformMatrix;

    void UpdateTransformMatrix() {

        transformMatrix = glm::translate(glm::mat4(1.0f), position) *
            glm::mat4_cast(glm::quat(rotation)) *
            glm::scale(glm::mat4(1.0f), scale);
    }
};

#endif // COMPONENTTRANSFORM_H
