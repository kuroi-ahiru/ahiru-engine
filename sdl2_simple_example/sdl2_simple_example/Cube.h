#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm/glm.hpp>

class Cube {
public:
    // V�rtices de un cubo (8 v�rtices)
    static const std::vector<glm::vec3> vertices;
    // �ndices para conectar los v�rtices y formar las caras (12 tri�ngulos)
    static const std::vector<unsigned int> indices;
    static const std::vector<glm::vec2> texCoords;
};

#endif // CUBE_H
