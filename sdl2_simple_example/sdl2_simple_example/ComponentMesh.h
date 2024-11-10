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
        : Component(Type::Mesh, owner), vertices(vertices), texCoords(texCoords), indices(indices), textureID(textureID), showNormals(false) {}

    void Update() override {}
    void Render() override {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBegin(GL_TRIANGLES);
        for (unsigned int i = 0; i < indices.size(); i++) {
            const glm::vec3& vertex = vertices[indices[i]];
            const glm::vec2& uv = texCoords[indices[i]];
            glTexCoord2f(uv.x, uv.y);
            glVertex3f(vertex.x, vertex.y, vertex.z);
        }
        glEnd();

        glDisable(GL_TEXTURE_2D);

        if (showNormals) {
            RenderNormals();
        }
    }

    // Getter y Setter para showNormals
    bool IsShowNormalsEnabled() const { return showNormals; }
    void SetShowNormals(bool show) { showNormals = show; }

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    GLuint textureID;

    bool showNormals;     
    void RenderNormals() {
        glColor3f(1.0f, 0.0f, 0.0f); //color rojo para las normales
        glBegin(GL_LINES);
        for (unsigned int i = 0; i < indices.size(); i += 3) {
            glm::vec3 v0 = vertices[indices[i]];
            glm::vec3 v1 = vertices[indices[i + 1]];
            glm::vec3 v2 = vertices[indices[i + 2]];

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));
            glm::vec3 center = (v0 + v1 + v2) / 3.0f; //centro del triangulo

            glVertex3f(center.x, center.y, center.z);
            glVertex3f(center.x + normal.x, center.y + normal.y, center.z + normal.z);
        }
        glEnd();
    }
};

#endif // COMPONENTMESH_H
