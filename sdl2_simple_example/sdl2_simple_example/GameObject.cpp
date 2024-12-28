#include "GameObject.h"
#include <glm/gtc/type_ptr.hpp> //para OpenGL
#include <limits> //para valores max y min, (AABB)



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

void GameObject::CalculateAABB() {
    //inicializa las esquinas min y max con valores extremos
    glm::vec3 minBounds(std::numeric_limits<float>::max());
    glm::vec3 maxBounds(std::numeric_limits<float>::lowest());

    //recorremos todos los componentes
    for (const auto& component : components) {
        if (component && component->GetType() == Component::Type::Mesh) {
            auto* meshComponent = dynamic_cast<ComponentMesh*>(component.get());
            if (meshComponent) {
                //obtener la AABB local del mesh
                glm::vec3 localMin = meshComponent->GetBoundingBoxMin();
                glm::vec3 localMax = meshComponent->GetBoundingBoxMax();

                //transformar extremos locales usando el GameObject transform
                for (int i = 0; i < 8; ++i) {
                    glm::vec3 corner = glm::vec3(
                        (i & 1) ? localMax.x : localMin.x,
                        (i & 2) ? localMax.y : localMin.y,
                        (i & 4) ? localMax.z : localMin.z
                    );

                    //aplicar transform
                    glm::vec4 transformedCorner = glm::vec4(corner, 1.0f);
                    for (const auto& transformComponent : components) {
                        if (transformComponent->GetType() == Component::Type::Transform) {
                            auto* transform = dynamic_cast<ComponentTransform*>(transformComponent.get());
                            if (transform) {
                                transformedCorner = transform->GetTransformMatrix() * transformedCorner;
                            }
                        }
                    }

                    //actualizar los v min y max globales
                    minBounds = glm::min(minBounds, glm::vec3(transformedCorner));
                    maxBounds = glm::max(maxBounds, glm::vec3(transformedCorner));
                }
            }
        }
    }

    //actualizar AABB del GameObject
    aabbMin = minBounds;
    aabbMax = maxBounds;
}

void GameObject::DrawAABB(const glm::vec3& color) {
    
    if (aabbMin == aabbMax) {
        return; // AABB no inicializado o degenerado
    }

    //propiedades de renderizado lineas
    glPushAttrib(GL_ENABLE_BIT | GL_LINE_BIT | GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glColor3f(color.r, color.g, color.b);

    //definir 8 esquinas de la AABB
    glm::vec3 corners[8];
    for (int i = 0; i < 8; ++i) {
        corners[i] = glm::vec3(
            (i & 1 ? aabbMax.x : aabbMin.x),
            (i & 2 ? aabbMax.y : aabbMin.y),
            (i & 4 ? aabbMax.z : aabbMin.z)
        );
    }

    //dibujar lineas
    glBegin(GL_LINES);
    int edges[12][2] = {
        {0, 1}, {1, 3}, {3, 2}, {2, 0}, //Base inferior
        {4, 5}, {5, 7}, {7, 6}, {6, 4}, //base superior
        {0, 4}, {1, 5}, {2, 6}, {3, 7}  //Conexiones verticales
    };
    for (int i = 0; i < 12; ++i) {
        glVertex3fv(glm::value_ptr(corners[edges[i][0]]));
        glVertex3fv(glm::value_ptr(corners[edges[i][1]]));
    }
    glEnd();

    //restaurar config previa
    glPopAttrib();
}

//void GameObject::DrawAABB() {
//    // Dibujar líneas usando las esquinas de la AABB
//    glBegin(GL_LINES);
//
//    // Definir las 8 esquinas de la AABB
//    glm::vec3 corners[8] = {
//        {aabbMin.x, aabbMin.y, aabbMin.z},
//        {aabbMin.x, aabbMin.y, aabbMax.z},
//        {aabbMin.x, aabbMax.y, aabbMin.z},
//        {aabbMin.x, aabbMax.y, aabbMax.z},
//        {aabbMax.x, aabbMin.y, aabbMin.z},
//        {aabbMax.x, aabbMin.y, aabbMax.z},
//        {aabbMax.x, aabbMax.y, aabbMin.z},
//        {aabbMax.x, aabbMax.y, aabbMax.z}
//    };
//
//    // Conectar las esquinas con líneas para formar el cubo
//    int edges[12][2] = {
//        {0, 1}, {1, 3}, {3, 2}, {2, 0}, // Base inferior
//        {4, 5}, {5, 7}, {7, 6}, {6, 4}, // Base superior
//        {0, 4}, {1, 5}, {2, 6}, {3, 7}  // Conexiones verticales
//    };
//
//    for (int i = 0; i < 12; ++i) {
//        glVertex3fv(glm::value_ptr(corners[edges[i][0]]));
//        glVertex3fv(glm::value_ptr(corners[edges[i][1]]));
//    }
//
//    glEnd();
//}