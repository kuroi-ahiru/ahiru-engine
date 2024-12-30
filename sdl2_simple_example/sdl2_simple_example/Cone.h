#ifndef CONE_H
#define CONE_H

#include <vector>
#include <glm/glm.hpp>

class Cone {
public:
    static const std::vector<glm::vec3> vertices;
    static const std::vector<glm::vec2> texCoords;
    static const std::vector<unsigned int> indices;
};

#endif // CONE_H
