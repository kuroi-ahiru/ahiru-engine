#ifndef CUBE_H
#define CUBE_H

#include <vector>
#include <glm/glm.hpp>

class Cube {
public:
    // Vértices de un cubo (8 vértices)
    static const std::vector<glm::vec3> vertices;
    // Índices para conectar los vértices y formar las caras (12 triángulos)
    static const std::vector<unsigned int> indices;
    static const std::vector<glm::vec2> texCoords;
};

#endif // CUBE_H
