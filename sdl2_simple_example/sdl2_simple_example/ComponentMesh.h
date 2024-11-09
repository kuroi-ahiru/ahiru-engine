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
        : Component(Type::Mesh, owner), vertices(vertices), texCoords(texCoords), indices(indices), textureID(textureID) {}

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
    }

private:
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<unsigned int> indices;
    GLuint textureID;
};

#endif // COMPONENTMESH_H
