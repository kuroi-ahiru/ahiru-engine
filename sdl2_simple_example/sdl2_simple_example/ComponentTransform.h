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
        : Component(Type::Transform, owner), position(position), rotation(rotation), scale(scale) {}

    void Update() override {}
    void Render() override {}

    // Getters para el inspector
    const glm::vec3& GetPosition() const { return position; }
    const glm::vec3& GetRotation() const { return rotation; }
    const glm::vec3& GetScale() const { return scale; }

    // Setters, marca el componente como dirty (dirty flags) para aplicar transformacion
    /*void SetPosition(const glm::vec3& newPos) { position = newPos; }
    void SetRotation(const glm::vec3& newRot) { rotation = newRot; }
    void SetScale(const glm::vec3& newScale) { scale = newScale; }*/

    void SetPosition(const glm::vec3& newPos) {
        position = newPos;
        isDirty = true;
    }
    void SetRotation(const glm::vec3& newRot) {
        rotation = newRot;
        isDirty = true;
    }
    void SetScale(const glm::vec3& newScale) {
        scale = newScale;
        isDirty = true;
    }

    //recalcular matriz en caso de que esté marcado como dirty
    const glm::mat4& GetTransformMatrix() {
        if (isDirty) {
            RecalculateTransformMatrix();
            isDirty = false;
        }
        return transformMatrix;
    }

private:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    glm::mat4 transformMatrix;
    bool isDirty;

    void RecalculateTransformMatrix() {
        glm::mat4 translation = glm::translate(glm::mat4(1.0f), position);
        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.x), glm::vec3(1, 0, 0));
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.y), glm::vec3(0, 1, 0));
        glm::mat4 rotationZ = glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));
        glm::mat4 scaling = glm::scale(glm::mat4(1.0f), scale);

        // Combinar las transformaciones: T * Rz * Ry * Rx * S
        transformMatrix = translation * rotationZ * rotationY * rotationX * scaling;
    }
};

#endif // COMPONENTTRANSFORM_H

