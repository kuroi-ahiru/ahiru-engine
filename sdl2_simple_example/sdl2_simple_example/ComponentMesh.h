#ifndef COMPONENTMESH_H
#define COMPONENTMESH_H

#include "Component.h"
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

class ComponentMesh : public Component {
public:
    ComponentMesh(GameObject* owner, const std::vector<glm::vec3>& vertices,
        const std::vector<glm::vec2>& texCoords, const std::vector<unsigned int>& indices, GLuint textureID)
        : Component(Type::Mesh, owner), vertices(vertices), texCoords(texCoords), indices(indices), textureID(textureID), showNormals(false), visible(true) {}

    void Update() override {}
    void Render() override {

        if (!visible) return;  // Si 'visible' es falso, no renderiza
        // Renderizado de la textura
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glColor3f(1.0f, 1.0f, 1.0f);  // Restablece el color a blanco antes del renderizado
        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < indices.size(); i++) 
        {
            const glm::vec3& vertex = vertices[indices[i]];
            const glm::vec2& uv = texCoords[indices[i]];
            glTexCoord2f(uv.x, uv.y);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);

        if (showNormals) {

            RenderNormals();
            glColor3f(1.0f, 1.0f, 1.0f); 
        }
    }

    // Getter y Setter showNormals
    bool IsShowNormalsEnabled() const { return showNormals; }
    void SetShowNormals(bool show) { showNormals = show; }

    // Getter y Setter activar y desactivar ek modelo
    bool IsVisible() const { return visible; }
    void SetVisible(bool isVisible) { visible = isVisible; }

    glm::vec3 GetBoundingBoxMin() const {
        glm::vec3 min(FLT_MAX);
        for (const auto& vertex : vertices) {
            min = glm::min(min, vertex);
        }
        return min;
    }

    glm::vec3 GetBoundingBoxMax() const {
        glm::vec3 max(-FLT_MAX);
        for (const auto& vertex : vertices) {
            max = glm::max(max, vertex);
        }
        return max;
    }

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    GLuint textureID;
    bool showNormals;
    bool visible;

    void RenderNormals() {
        glColor3f(1.0f, 0.0f, 0.0f); // Color rojo para las normales
        glBegin(GL_LINES);
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            glm::vec3 v0 = vertices[indices[i]];
            glm::vec3 v1 = vertices[indices[i + 1]];
            glm::vec3 v2 = vertices[indices[i + 2]];

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 center = (v0 + v1 + v2) / 3.0f; // Centro del triángulo

            glVertex3f(center.x, center.y, center.z);
            glVertex3f(center.x + normal.x, center.y + normal.y, center.z + normal.z);
        }
        glEnd();
    }
};

#endif // COMPONENTMESH_H
